// ld_tv_check_not_unsat's reference, correlated without an in entry: the two sides'
// inputs are independent, so the outputs can differ.
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
