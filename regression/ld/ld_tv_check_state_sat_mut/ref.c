// Latch with the hold dropped (q = in): equal to Latch from the initial state,
// but not from a pre-state where q is already set, which the state entry
// reaches.
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
