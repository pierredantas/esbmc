struct ref_data { _Bool in; _Bool out; };
int ref_scan(void)
{
  struct ref_data d;
  d.in = nondet_bool();
  d.out = !d.in;
  return 0;
}
