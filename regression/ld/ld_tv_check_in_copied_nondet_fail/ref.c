// d.in2 is a copy of d.in, not a read of its own nondet. Constant propagation
// makes its first write store the same nondet symbol as d.in's, so tying both
// In1 and In2 to it would force In1 == In2 and make the LD output constant 0.
// ld-tv-check rejects the second tie instead of reporting a false UNSAT.
struct ref_data
{
  _Bool in;
  _Bool in2;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.in2 = d.in;
  d.out = d.in && !d.in2;
  return 0;
}
