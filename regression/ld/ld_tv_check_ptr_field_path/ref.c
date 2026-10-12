// d.out is written once through a pointer alias (p->out = ...) and once
// through a cast of that same pointer (((struct ref_data *)(void *)p)->out).
// final_value_of matches on the renamed lhs, so both writes reach it. There is
// no in entry: with d address-taken, its nondet-fed write is not found and an
// in entry would fail closed. The input stays untied, hence SAT.
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
