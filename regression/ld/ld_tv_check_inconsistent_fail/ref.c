// d.inv is the negation of d.in, and the correlation file ties both to In1.
// No run has In1 equal to both, so the constraints are inconsistent before any
// comparison and an UNSAT verdict would be vacuous.
struct ref_data
{
  _Bool in;
  _Bool inv;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.inv = !d.in;
  d.out = !d.in;
  return 0;
}
