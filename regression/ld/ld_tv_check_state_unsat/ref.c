// Hand-written reference for Latch (Q is set by In1 and never cleared). The
// state entry leaves d.q arbitrary before the scan, so a reference that
// forgot the latch would disagree on the pre-states where q was already set.
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
  d.q = d.q || d.in;
  return 0;
}
