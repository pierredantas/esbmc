// ld_tv_check_in_unsat's reference with an unrelated nondet read first. The in entry
// ties d.in to In1 by variable, not by read order.
struct ref_data
{
  _Bool in;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  _Bool unused = nondet_bool();
  (void)unused;
  d.in = nondet_bool();
  d.out = !d.in;
  return 0;
}
