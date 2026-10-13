#pragma once

#include <ld-frontend/parser/ld_ast.h>
#include <ld-frontend/property/yaml_property_parser.h>
#include <util/symtab/context.h>
#include <util/irep/std_code.h>
#include <util/irep/std_expr.h>
#include <map>
#include <string>
#include <vector>

// property_encoder translates LdProperty objects into code_assertt nodes
// which are appended to the end of the scan-loop body by ld_converter.
class property_encoder
{
public:
  property_encoder(contextt &context, const std::string &source_file);

  // Encode all properties and return a code_blockt of assertions.
  // Throws std::runtime_error if a property references an undeclared variable
  // or uses a variable that is not BOOL as an atom.
  code_blockt encode(const std::vector<LdProperty> &props);

  // EXPERIMENT (--ld-monitor-invariants): the program the response monitors may
  // be related to. The relations found are returned by monitor_facts(); the
  // caller assumes each at the start of the scan loop and asserts it once
  // before the loop.
  void set_program(const std::vector<NetworkNode> &networks);
  const std::vector<exprt> &monitor_facts() const
  {
    return monitor_facts_;
  }

private:
  contextt &context_;
  std::string source_file_;
  struct TimerSite
  {
    TimerFBNode timer;
    size_t rung;
  };
  // A variable driven by one rung that is a single contact and one coil, and by
  // nothing else: an alias of the contact's variable, possibly negated.
  struct Alias
  {
    std::string source;
    bool negated;
    size_t rung;
  };
  struct Literal
  {
    std::string var;
    bool positive = true;
    bool aliased = false;
    size_t first_rung = 0; // the earliest rung among the aliases followed
  };
  std::vector<TimerSite> timers_;
  std::map<std::string, Alias> aliases_;
  std::vector<exprt> monitor_facts_;

  symbol_exprt var_expr(const std::string &name, const LdProperty &p) const;
  symbol_exprt bool_var(const std::string &name, const LdProperty &p) const;
  exprt parse_bool_expr(const std::string &expr_str, const LdProperty &p) const;

  code_assertt make_assert(const exprt &cond, const LdProperty &p) const;

  code_blockt encode_mutual_exclusion(const LdProperty &p);
  code_blockt encode_invariant(const LdProperty &p);
  code_blockt encode_absence(const LdProperty &p);
  code_blockt encode_reachability(const LdProperty &p);
  code_blockt encode_response(const LdProperty &p);
  Literal resolve(const std::string &var) const;
  bool watches(const LdProperty &p, const TimerSite &s) const;
  exprt timer_monitor_fact(
    const LdProperty &p,
    const symbol_exprt &ctr,
    const TimerFBNode &t);
};
