// d.in is assigned a constant, never a nondet, so the in entry has no
// reference-side write to tie to In1.
struct ref_data
{
  _Bool in;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = 1;
  d.out = !d.in;
  return 0;
}
