struct inner_t { _Bool a; _Bool b; };
struct ref_data { struct inner_t inner; _Bool out; };
struct inner_t nondet_inner(void);
int ref_scan(void)
{
  struct ref_data d = {0};
  d.inner = nondet_inner();
  d.out = !d.inner.a;
  return 0;
}
