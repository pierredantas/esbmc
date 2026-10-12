// The dropped-hold reference of ld_tv_check_state_sat_mut under an out entry:
// both sides start from q = 0, where the two agree, so ld-tv-check reports
// UNSAT. Only a state entry exposes the difference.
struct ref_data
{
  _Bool in;
  _Bool q;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.q = nondet_bool();
  d.in = nondet_bool();
  d.q = d.in;
  return 0;
}
