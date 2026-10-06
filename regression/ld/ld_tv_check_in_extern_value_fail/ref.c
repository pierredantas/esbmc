struct ref_data
{
  _Bool in;
  _Bool out;
};
extern _Bool v;
int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = v;
  d.out = !d.in;
  return 0;
}
