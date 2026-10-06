struct ref_data { _Bool a; _Bool out; };
struct other { _Bool zz; };
int ref_scan(void)
{
  struct ref_data s = {0};
  struct other *p = (struct other *)&s;
  p->zz = nondet_bool();
  s.out = !p->zz;
  return 0;
}
