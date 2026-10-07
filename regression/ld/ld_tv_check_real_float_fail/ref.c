// Hand-written reference, as ld_tv_check_in_unsat. The LD side adds two REAL
// values as doubles; a float sum does not, because rounding to single precision
// can make two different sums equal.
struct ref_data
{
  float a;
  float b;
  float c;
  _Bool q;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.a = nondet_float();
  d.b = nondet_float();
  d.c = nondet_float();
  d.q = d.a + d.b == d.c;
  return 0;
}
