struct ref_data
{
  _Bool in;
  _Bool out;
};
int ref_scan(void)
{
  struct ref_data d = {0};
  _Bool *p = &d.out;
  *(unsigned char *)p = 1;
  d.in = nondet_bool();
  d.out = !d.in;
  return 0;
}
