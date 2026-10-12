// A hand-written stand-in for a MATIEC-generated C reference (WS1 item 4):
// no MATIEC runtime headers, so this fixture carries no external toolchain
// dependency. ref_scan computes out the same way NotGate's rung does
// (Out1 = !In1), so ld-tv-check's correlation of ld::Out1 against d.out
// reports UNSAT.
struct ref_data
{
  _Bool in;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.out = !d.in;
  return 0;
}
