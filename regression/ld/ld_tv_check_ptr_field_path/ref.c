// d.out is written once through a pointer alias (p->out = ...) and once
// through a cast of that same pointer (((struct ref_data *)(void *)p)->out),
// so final_value_of's decompose_field_path walk over every SSA assignment's
// original_lhs exercises its is_dereference2t and is_typecast2t unwrap
// branches (ld_tv_check.cpp), not just the plain-member case
// ld_tv_check_not_unsat/ref.c already covers. This fixture's goal is
// exercising that parser path, not pinning a specific UNSAT/SAT outcome;
// ESBMC's pointer model for this dynamic-object write produces SAT here,
// confirmed empirically rather than predicted from the rung's logic.
struct ref_data
{
  _Bool in;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  struct ref_data *p = &d;
  d.in = nondet_bool();
  p->out = !d.in;
  ((struct ref_data *)(void *)p)->out = !d.in;
  return 0;
}
