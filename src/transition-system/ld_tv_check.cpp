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

// Each symex_one_step call runs its own goto_symext from a clean renaming
// state, so both passes produce the same SSA names for low-numbered temporaries
// (goto_symex::guard?0!0&0#1, nondet$symex::nondet0, a global's first
// version). One shared smt_convt would alias them silently. Every symbol of
// eq_ref is therefore moved out of eq_ld's namespace: level1/level2 symbols by
// thread_num (eq_ld only ever runs thread 0), global symbols, which carry no
// thread in their name, by a name prefix. Correlation between the sides is
// then only what the correlation file states.
constexpr unsigned ref_pass_thread_tag = 1;
const std::string ref_pass_prefix = "ref::";

// expr2tc nodes are hash-consed, so a symbol is rebuilt and the slot
// reassigned, as goto_symext::replace_nondet (symex_assign.cpp) does.
void retag_ref_symbols(expr2tc &e)
{
  if (is_nil_expr(e))
    return;
  if (is_symbol2t(e))
  {
    const symbol2t &sym = to_symbol2t(e);
    const bool global =
      sym.rlevel == symbol2t::renaming_level::level1_global ||
      sym.rlevel == symbol2t::renaming_level::level2_global;
    if (
      !global && sym.rlevel != symbol2t::renaming_level::level1 &&
      sym.rlevel != symbol2t::renaming_level::level2)
      return;
    e = symbol2tc(
      sym.type,
      global ? ref_pass_prefix + sym.thename.as_string()
             : sym.thename.as_string(),
      sym.rlevel,
      sym.level1_num,
      sym.level2_num,
      global ? sym.thread_num : ref_pass_thread_tag,
      sym.node_num);
    return;
  }
  e.get()->Foreach_operand([](expr2tc &op) { retag_ref_symbols(op); });
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
bool thename_matches(std::string thename, const std::string &base_symbol)
{
  if (thename.rfind(ref_pass_prefix, 0) == 0)
    thename.erase(0, ref_pass_prefix.size());
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
    // The renamed lhs names the whole struct even when the write went through
    // a pointer (a MATIEC body's data__->Y), where original_lhs is only a
    // dereference.
    if (
      step.is_assignment() && is_symbol2t(step.lhs) &&
      thename_matches(to_symbol2t(step.lhs).thename.as_string(), base_symbol))
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

bool mentions_nondet(const expr2tc &e)
{
  if (is_nil_expr(e))
    return false;
  if (is_symbol2t(e))
    return to_symbol2t(e).thename.as_string().find("nondet") !=
           std::string::npos;
  bool found = false;
  e->foreach_operand(
    [&found](const expr2tc &op) { found = found || mentions_nondet(op); });
  return found;
}

// The member at `members` inside a constant struct `rhs`, or nil when `rhs`
// is not a literal of that shape.
expr2tc rhs_field(const expr2tc &rhs, const std::vector<irep_idt> &members)
{
  expr2tc cur = rhs;
  for (const irep_idt &name : members)
  {
    if (is_nil_expr(cur) || !is_constant_struct2t(cur))
      return expr2tc();
    const struct_type2t &st = to_struct_type(cur->type);
    auto it = std::find(st.member_names.begin(), st.member_names.end(), name);
    if (it == st.member_names.end())
      return expr2tc();
    cur = to_constant_struct2t(cur).datatype_members[it - st.member_names.begin()];
  }
  return cur;
}

// The value `base.members` takes at the first write that stores a nondet
// into it: an input read, or the havoc a state entry plants. Constant
// initialisers are skipped. Nil when no such write exists.
expr2tc first_nondet_write(
  const symex_target_equationt &eq,
  const std::string &base,
  const std::vector<irep_idt> &members)
{
  for (const auto &step : eq.SSA_steps)
  {
    if (!step.is_assignment() || !step.original_lhs)
      continue;
    auto path = decompose_field_path(step.original_lhs);
    if (
      !path || !thename_matches(path->base_symbol, base) ||
      path->members != members || !mentions_nondet(rhs_field(step.rhs, members)))
      continue;
    return members.empty() ? step.lhs : project_field_path(step.lhs, members);
  }
  return expr2tc();
}

// Replaces the first assignment to `ld_var` in `main`, the LD side's
// initialiser, with a nondet, so the scan starts from any value of it.
bool havoc_initial_value(goto_functiont &main, const std::string &ld_var)
{
  for (auto &instr : main.body.instructions)
  {
    if (!instr.is_assign())
      continue;
    code_assign2t &assign = to_code_assign2t(instr.code);
    if (
      is_symbol2t(assign.target) &&
      thename_matches(to_symbol2t(assign.target).thename.as_string(), ld_var))
    {
      assign.source = gen_nondet(assign.target->type);
      return true;
    }
  }
  return false;
}

enum class correlation_kindt
{
  out,   // post-scan values must agree
  in,    // both sides read the same input value
  state, // both sides start from the same arbitrary value, and agree after
};

struct correlation_entryt
{
  correlation_kindt kind = correlation_kindt::out;
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
    std::vector<std::string> tokens;
    for (std::string tok; ls >> tok;)
      tokens.push_back(tok);
    if (tokens.empty() || tokens[0][0] == '#')
      continue;
    correlation_kindt kind = correlation_kindt::out;
    if (tokens.size() == 3)
    {
      if (tokens[0] == "in")
        kind = correlation_kindt::in;
      else if (tokens[0] == "state")
        kind = correlation_kindt::state;
      else if (tokens[0] != "out")
      {
        log_error(
          "ld-tv-check: {}:{}: '{}' is not one of in, out, state",
          path, lineno, tokens[0]);
        return std::nullopt;
      }
      tokens.erase(tokens.begin());
    }
    if (tokens.size() != 2)
    {
      log_error(
        "ld-tv-check: {}:{}: expected \"[in|out|state] ld_var ref.field.path\"",
        path, lineno);
      return std::nullopt;
    }
    const std::string &ld_var = tokens[0];
    const std::string &ref_spec = tokens[1];
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
    table.push_back({kind, ld_var, ref_var, path_parts});
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

  const std::string correlation_path = options.get_option("ld-tv-check");
  auto table = read_correlation_file(correlation_path);
  if (!table)
    return 1;
  const bool compares = std::any_of(
    table->begin(), table->end(), [](const correlation_entryt &e) {
      return e.kind != correlation_kindt::in;
    });
  if (!compares)
  {
    log_error(
      "ld-tv-check: '{}' names no out or state variable to compare",
      correlation_path);
    return 1;
  }

  // Pass 1: the LD side. A state entry starts from an arbitrary value, so its
  // initialiser becomes a nondet; the original body is restored afterwards.
  goto_functiont ld_body_saved = ld_main->second;
  for (const auto &entry : *table)
  {
    if (
      entry.kind == correlation_kindt::state &&
      !havoc_initial_value(ld_main->second, entry.ld_var))
    {
      ld_main->second = ld_body_saved;
      log_error(
        "ld-tv-check: '{}' is never initialised on the LD side, so it has no "
        "initial value to leave arbitrary",
        entry.ld_var);
      return 1;
    }
  }
  auto eq_ld = symex_one_step(goto_functions, context, options);
  if (!eq_ld)
  {
    ld_main->second = ld_body_saved;
    log_error("ld-tv-check: symex of the LD side produced no equation");
    return 1;
  }

  // Pass 2: swap the reference body into __ESBMC_main's slot so the same
  // (otherwise hardcoded, see execution_statet's constructor) entry point
  // reaches it, then restore the LD body before returning: a later caller
  // of goto_functions (unlikely in this strategy, but cheap to keep clean)
  // should see the program it was handed.
  ld_main->second.body = ref_main->second.body;
  auto eq_ref = symex_one_step(goto_functions, context, options);
  ld_main->second = ld_body_saved;

  if (!eq_ref)
  {
    log_error("ld-tv-check: symex of the reference side produced no equation");
    return 1;
  }

  // Keep the two passes' symbols apart before either reaches the shared solver.
  for (auto &step : eq_ref->SSA_steps)
  {
    retag_ref_symbols(step.guard);
    retag_ref_symbols(step.lhs);
    retag_ref_symbols(step.rhs);
    retag_ref_symbols(step.original_lhs);
    retag_ref_symbols(step.original_rhs);
    retag_ref_symbols(step.cond);
    retag_ref_symbols(step.cond_neg);
    retag_ref_symbols(step.cond_expr);
  }

  namespacet ns(context);
  optionst solver_opts = options;
  std::unique_ptr<smt_convt> solver(create_solver("", ns, solver_opts));

  // Both equations' assignments and assumes become hard constraints. The LD
  // side's own property assertions (from --ld-props) are dropped: converted
  // in Violated mode they would negate a property that holds and make the
  // whole query UNSAT, whatever the translation does.
  for (auto *eq : {eq_ld.get(), eq_ref.get()})
    for (auto &step : eq->SSA_steps)
      if (step.is_assert())
        step.ignore = true;
  eq_ld->convert(*solver, symex_target_equationt::assertion_modet::Violated);
  eq_ref->convert(*solver, symex_target_equationt::assertion_modet::Violated);

  std::vector<expr2tc> mismatches;
  for (const auto &entry : *table)
  {
    if (entry.kind != correlation_kindt::out)
    {
      expr2tc ld_in = first_nondet_write(*eq_ld, entry.ld_var, {});
      expr2tc ref_in =
        first_nondet_write(*eq_ref, entry.ref_var, entry.ref_path);
      if (!ld_in || !ref_in)
      {
        log_error(
          "ld-tv-check: no nondet-fed write to {} found on the {} side; an "
          "in or state entry needs the reference to assign a nondet to its "
          "variable before the scan",
          !ld_in ? entry.ld_var : entry.ref_var,
          !ld_in ? "LD" : "reference");
        return 1;
      }
      if (ld_in->type != ref_in->type)
        ref_in = typecast2tc(ld_in->type, ref_in);
      solver->assert_expr(equality2tc(ld_in, ref_in));
      if (entry.kind == correlation_kindt::in)
        continue;
    }
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

  // The query is only meaningful if the two equations and the correlation
  // equalities admit some run: an inconsistent set would make every verdict
  // UNSAT.
  if (solver->dec_solve() != P_SATISFIABLE)
  {
    log_error(
      "ld-tv-check: the two sides' constraints admit no run before any "
      "comparison, so an UNSAT verdict would be vacuous");
    return 1;
  }
  solver->assert_expr(disjunction(mismatches));

  log_status(
    "ld-tv-check: {} correlated variable(s); solving", table->size());
  switch (solver->dec_solve())
  {
  case P_UNSATISFIABLE:
    log_result(
      "LD-TV-CHECK UNSAT: every out and state variable agrees after one "
      "scan, for every value of the in and state variables; all other state "
      "starts at its initial value on both sides");
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
