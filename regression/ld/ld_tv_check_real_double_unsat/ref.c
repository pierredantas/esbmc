// Hand-written reference, as ld_tv_check_in_unsat. The LD side adds two REAL
// values as doubles; a double sum agrees with it.
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
  d.q = (double)d.a + (double)d.b == (double)d.c;
  return 0;
}
