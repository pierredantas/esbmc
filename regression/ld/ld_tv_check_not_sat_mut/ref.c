// Same hand-written reference as ld_tv_check_not_unsat/ref.c, but d.out is
// left as the identity of d.in instead of its negation: this mutates the
// reference away from NotGate's rung (Out1 = !In1), so ld-tv-check's
// correlation of ld::Out1 against d.out finds a disagreeing pre-state and
// reports SAT.
struct ref_data
{
  _Bool in;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.out = d.in;
  return 0;
}
