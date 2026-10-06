#include <transition-system/ld_tv_check.h>

#include <goto-symex/equation/symex_target_equation.h>
#include <goto-symex/scheduler/reachability_tree.h>
#include <solvers/smt/smt_conv.h>
#include <solvers/solve.h>
#include <util/message/message.h>

#include <algorithm>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>

namespace
{
// One pass of goto-symex from __ESBMC_main, unwound once. Mirrors
// unit/goto-symex/symex_run.h's `equation`, which cannot be reused directly:
// that header builds its own goto_functions from a C source string, where
// this driver is handed an already built, already combined program.
std::shared_ptr<symex_target_equationt> symex_one_step(
  goto_functionst &goto_functions,
  contextt &context,
  optionst &options)
{
  namespacet ns(context);
  optionst opts = options;
  opts.set_option("unwind", "1");
  opts.set_option("no-unwinding-assertions", true);
  opts.set_option("smt-during-symex", false);
  opts.set_option("schedule", false);
  opts.set_option("multi-property", false);

  reachability_treet art(
    goto_functions,
    ns,
    opts,
    std::make_shared<symex_target_equationt>(ns),
    context);
  art.setup_for_new_explore();
  std::shared_ptr<symex_targett> target = art.get_next_formula().target;
  return std::dynamic_pointer_cast<symex_target_equationt>(target);
}

// Each symex_one_step call below runs its own goto_symext from a clean
// level1t/level2t renaming state, independently starting its l1/l2/node
// counters from 0 (confirmed: neither pass is multithreaded, so
// level1.thread_id is 0 in both). get_symbol_name()'s fully-qualified SSA
// name is a pure function of (thename, rlevel, l1, thread, node, l2); two
// structurally similar programs (an LD scan vs. its reference translation)
// readily produce the SAME such tuple in both passes for low-numbered,
// per-frame temporaries (goto_symext::guard_identifier()'s own
// "goto_symex::guard" at level1, l1=0, thread=0 is one, confirmed
// unconditionally constructed this way by every symex run). Converting
// both equations into one shared smt_convt (as this tool does, to assert
// cross-equation equalities) would then silently alias the two passes'
// unrelated guards/temporaries onto the same SMT constant, corrupting the
// comparison without erroring. Fixed by retagging every level1/level2
// symbol2t in eq_ref's SSA steps to a thread_num no level1/level2 symbol
// in eq_ld can have (eq_ld's symex only ever runs at thread 0); level1/
// level2's own get_symbol_name() branches include thread_num in the built
// name (level1_global/level2_global do not, but those name truly shared
// globals, e.g. pthread bookkeeping state, which this single-threaded tool
// is not at risk of needing to keep the two passes' views of distinct).
constexpr unsigned ref_pass_thread_tag = 1;

// Rebuilds `e`'s level1/level2 symbol2t nodes with thread_num retagged to
// `tag`, leaving level0/level1_global/level2_global symbols untouched (see
// the comment above this function). expr2tc nodes are hash-consed, so a
// matched symbol2t is never mutated in place; the pattern mirrors
// goto_symext::replace_nondet (symex_assign.cpp), which rebuilds and
// reassigns the expr2tc& slot instead of mutating a shared node.
void retag_thread_num(expr2tc &e, unsigned tag)
{
  if (is_nil_expr(e))
    return;
  if (is_symbol2t(e))
  {
    const symbol2t &sym = to_symbol2t(e);
    if (
      sym.rlevel == symbol2t::renaming_level::level1 ||
      sym.rlevel == symbol2t::renaming_level::level2)
      e = symbol2tc(
        sym.type,
        sym.thename,
        sym.rlevel,
        sym.level1_num,
        sym.level2_num,
        tag,
        sym.node_num);
    return;
  }
  e.get()->Foreach_operand(
    [tag](expr2tc &op) { retag_thread_num(op, tag); });
}

// Unwraps member/dereference/typecast layers from `e`, matching §6 of
// ws1/tv_harness_design.md: a struct write's `original_lhs` is a chain of
// these over a base symbol, confirmed against a real produced equation
// (ws1/spikes/tv_spike_g_ton_single_ssa_dump.txt). Returns the member names
// in outer-to-inner order (so `d.TON0.Q.value` gives {"TON0", "Q", "value"})
// and the base symbol's name, or nullopt if `e` is not one of these shapes.
struct field_patht
{
  std::string base_symbol;
  std::vector<irep_idt> members; // outermost first
};

std::optional<field_patht> decompose_field_path(const expr2tc &e)
{
  std::vector<irep_idt> members;
  expr2tc cur = e;
  while (true)
  {
    if (!cur)
      return std::nullopt;
    if (is_symbol2t(cur))
    {
      std::reverse(members.begin(), members.end());
      // thename is the pre-SSA identity (e.g. "c:@F@ref_scan@d" for a C
      // local, "ld::v" for an LD variable), not get_symbol_name()'s fully
      // renamed form (e.g. "c:test.c@...@main@d?1!0&0#N"); confirmed
      // against a real produced equation
      // (ws1/spikes/tv_spike_g_ton_single_ssa_dump.txt), where even
      // original_lhs's base symbol is SSA-renamed, contrary to this
      // file's own earlier assumption.
      return field_patht{to_symbol2t(cur).thename.as_string(), members};
    }
    if (is_member2t(cur))
    {
      const member2t &m = to_member2t(cur);
      members.push_back(m.member);
      cur = m.source_value;
      continue;
    }
    if (is_dereference2t(cur))
    {
      cur = to_dereference2t(cur).value;
      continue;
    }
    if (is_typecast2t(cur))
    {
      cur = to_typecast2t(cur).from;
      continue;
    }
    return std::nullopt;
  }
}

// True when `thename` is exactly `base_symbol`, or ends with it right
// after a "@" (so the correlation file can write the short local name,
// e.g. "d", and match the real symbol id "c:@F@ref_scan@d", without the
// caller having to know or spell the enclosing function).
bool thename_matches(const std::string &thename, const std::string &base_symbol)
{
  if (thename == base_symbol)
    return true;
  const std::string suffix = "@" + base_symbol;
  return thename.size() > suffix.size() &&
         thename.compare(
           thename.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// The last assignment step whose original_lhs resolves to `base_symbol`
// (matched on thename, the pre-SSA identity; see decompose_field_path) is
// the final value of that whole struct/variable, per §6's confirmed
// technique: every write to one of its fields rewrites it whole, so the
// last such write's renamed `lhs` is strictly the final state.
expr2tc final_value_of(
  const symex_target_equationt &eq,
  const std::string &base_symbol)
{
  expr2tc last;
  for (const auto &step : eq.SSA_steps)
  {
    if (!step.is_assignment() || !step.original_lhs)
      continue;
    auto path = decompose_field_path(step.original_lhs);
    if (path && thename_matches(path->base_symbol, base_symbol))
      last = step.lhs;
  }
  return last;
}

// Builds a fresh member2t chain on `root` for `members` (outermost first),
// looking up each member's declared type from the enclosing struct type so
// member2t's result type is correct. Returns nullptr on a lookup miss
// (names this driver's own correlation table got wrong, not a program
// error), so the caller can report which field name failed to resolve.
expr2tc
project_field_path(const expr2tc &root, const std::vector<irep_idt> &members)
{
  expr2tc cur = root;
  for (const irep_idt &name : members)
  {
    type2tc struct_ty = cur->type;
    if (is_pointer_type(struct_ty))
      struct_ty = to_pointer_type(struct_ty).subtype;
    if (!is_struct_type(struct_ty))
      return expr2tc();
    const struct_type2t &st = to_struct_type(struct_ty);
    auto it =
      std::find(st.member_names.begin(), st.member_names.end(), name);
    if (it == st.member_names.end())
      return expr2tc();
    size_t idx = std::distance(st.member_names.begin(), it);
    cur = member2tc(st.members[idx], cur, name);
  }
  return cur;
}

struct correlation_entryt
{
  std::string ld_var;  // e.g. "ld::Light"
  std::string ref_var;  // the reference side's own top-level local, e.g. "d"
  std::vector<irep_idt> ref_path; // e.g. {"Light", "value"}
};

// One line is "ld_var ref_var.field1.field2...", whitespace-separated;
// blank lines and lines starting with # are skipped. Returns nullopt and
// logs on a malformed line, so a typo in the file is caught here rather
// than silently correlating the wrong thing.
std::optional<std::vector<correlation_entryt>>
read_correlation_file(const std::string &path)
{
  std::ifstream in(path);
  if (!in)
  {
    log_error("ld-tv-check: cannot open correlation file '{}'", path);
    return std::nullopt;
  }
  std::vector<correlation_entryt> table;
  std::string line;
  unsigned lineno = 0;
  while (std::getline(in, line))
  {
    ++lineno;
    std::istringstream ls(line);
    std::string ld_var, ref_spec;
    if (!(ls >> ld_var))
      continue; // blank line
    if (ld_var[0] == '#')
      continue;
    if (!(ls >> ref_spec))
    {
      log_error(
        "ld-tv-check: {}:{}: expected \"ld_var ref.field.path\"", path,
        lineno);
      return std::nullopt;
    }
    std::vector<irep_idt> path_parts;
    std::string ref_var;
    std::istringstream ps(ref_spec);
    std::string part;
    while (std::getline(ps, part, '.'))
    {
      if (ref_var.empty())
        ref_var = part;
      else
        path_parts.push_back(part);
    }
    if (ref_var.empty() || path_parts.empty())
    {
      log_error(
        "ld-tv-check: {}:{}: '{}' needs at least one field after the "
        "reference variable",
        path, lineno, ref_spec);
      return std::nullopt;
    }
    table.push_back({ld_var, ref_var, path_parts});
  }
  return table;
}
} // namespace

int run_ld_tv_check(
  contextt &context,
  optionst &options,
  goto_functionst &goto_functions)
{
  if (!options.get_bool_option("secondary-entry-point"))
  {
    log_error("--ld-tv-check needs --secondary-entry-point");
    return 1;
  }

  auto ld_main = goto_functions.function_map.find("__ESBMC_main");
  auto ref_main = goto_functions.function_map.find("__ESBMC_secondary_main");
  if (
    ld_main == goto_functions.function_map.end() ||
    ref_main == goto_functions.function_map.end())
  {
    log_error(
      "--ld-tv-check needs both __ESBMC_main and __ESBMC_secondary_main; "
      "run with an LD file and a reference file together");
    return 1;
  }

  // Pass 1: the LD side, as built. __ESBMC_main already calls
  // ld::scan_loop; nothing to swap.
  auto eq_ld = symex_one_step(goto_functions, context, options);
  if (!eq_ld)
  {
    log_error("ld-tv-check: symex of the LD side produced no equation");
    return 1;
  }

  // Pass 2: swap the reference body into __ESBMC_main's slot so the same
  // (otherwise hardcoded, see execution_statet's constructor) entry point
  // reaches it, then restore the LD body before returning: a later caller
  // of goto_functions (unlikely in this strategy, but cheap to keep clean)
  // should see the program it was handed.
  goto_functiont ld_body_saved = ld_main->second;
  ld_main->second.body = ref_main->second.body;
  auto eq_ref = symex_one_step(goto_functions, context, options);
  ld_main->second = ld_body_saved;

  if (!eq_ref)
  {
    log_error("ld-tv-check: symex of the reference side produced no equation");
    return 1;
  }

  // See retag_thread_num's own comment: disjoint the two equations'
  // level1/level2 symbol namespaces before either reaches the shared
  // solver, so same-named per-frame temporaries (symex's own
  // "goto_symex::guard" among them) from the two independent symex runs
  // are never aliased onto one SMT constant.
  for (auto &step : eq_ref->SSA_steps)
  {
    retag_thread_num(step.guard, ref_pass_thread_tag);
    retag_thread_num(step.lhs, ref_pass_thread_tag);
    retag_thread_num(step.rhs, ref_pass_thread_tag);
    retag_thread_num(step.original_lhs, ref_pass_thread_tag);
    retag_thread_num(step.original_rhs, ref_pass_thread_tag);
    retag_thread_num(step.cond, ref_pass_thread_tag);
    retag_thread_num(step.cond_neg, ref_pass_thread_tag);
    retag_thread_num(step.cond_expr, ref_pass_thread_tag);
  }

  const std::string correlation_path = options.get_option("ld-tv-check");
  auto table = read_correlation_file(correlation_path);
  if (!table)
    return 1;
  if (table->empty())
  {
    log_error(
      "ld-tv-check: '{}' names no variables to correlate", correlation_path);
    return 1;
  }

  namespacet ns(context);
  optionst solver_opts = options;
  std::unique_ptr<smt_convt> solver(create_solver("", ns, solver_opts));

  // Both equations' assignments/assumes become hard constraints (neither
  // program carries its own __ESBMC_assert, so convert()'s own assertion
  // encoding, whatever mode is passed, never fires; see
  // ws1/tv_harness_design.md §3 step 1).
  eq_ld->convert(*solver, symex_target_equationt::assertion_modet::Violated);
  eq_ref->convert(*solver, symex_target_equationt::assertion_modet::Violated);

  std::vector<expr2tc> mismatches;
  for (const auto &entry : *table)
  {
    expr2tc ld_final = final_value_of(*eq_ld, entry.ld_var);
    if (!ld_final)
    {
      log_error(
        "ld-tv-check: '{}' is never written in the LD side's scan; check "
        "the correlation file and the LD program's variable names",
        entry.ld_var);
      return 1;
    }
    expr2tc ref_final = final_value_of(*eq_ref, entry.ref_var);
    if (!ref_final)
    {
      log_error(
        "ld-tv-check: '{}' is never written on the reference side; check "
        "the correlation file's reference variable name",
        entry.ref_var);
      return 1;
    }
    expr2tc ref_value = project_field_path(ref_final, entry.ref_path);
    if (!ref_value)
    {
      log_error(
        "ld-tv-check: the reference side's '{}' has no field path '{}'; "
        "check the correlation file",
        entry.ref_var,
        [&entry] {
          std::string s;
          for (const auto &m : entry.ref_path)
            s += "." + m.as_string();
          return s;
        }());
      return 1;
    }
    if (ld_final->type != ref_value->type)
    {
      // LD's own BOOL is ESBMC's native 1-bit bool_t(), where MATIEC's
      // generated C represents IEC BOOL as uint8_t (iec_types.h); both are
      // scalar truth/numeric values, so a typecast suffices rather than
      // naming them incomparable.
      bool ld_scalar = is_bool_type(ld_final->type) || is_number_type(ld_final->type);
      bool ref_scalar = is_bool_type(ref_value->type) || is_number_type(ref_value->type);
      if (!ld_scalar || !ref_scalar)
      {
        log_error(
          "ld-tv-check: '{}' and '{}' have different, non-scalar types "
          "after symex; check the correlation file's field path",
          entry.ld_var, entry.ref_var);
        return 1;
      }
      ref_value = typecast2tc(ld_final->type, ref_value);
    }
    mismatches.push_back(not2tc(equality2tc(ld_final, ref_value)));
  }

  solver->assert_expr(disjunction(mismatches));

  log_status(
    "ld-tv-check: {} correlated variable(s); solving", table->size());
  switch (solver->dec_solve())
  {
  case P_UNSATISFIABLE:
    log_result(
      "LD-TV-CHECK UNSAT: every correlated variable agrees after one scan, "
      "for every pre-state and every nondet choice each side's symex made "
      "independently (see --help on --ld-tv-check: inputs are not yet "
      "correlated across the two sides)");
    break;
  case P_SATISFIABLE:
    log_result(
      "LD-TV-CHECK SAT: the LD side and the reference disagree on at "
      "least one correlated variable for some input/pre-state (this first "
      "version does not yet print which one or the counterexample; rerun "
      "with one correlation entry at a time to narrow it down)");
    break;
  default:
    log_error("ld-tv-check: the solver returned an error");
    return 1;
  }
  return 0;
}
