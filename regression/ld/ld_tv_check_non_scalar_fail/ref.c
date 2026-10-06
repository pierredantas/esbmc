// d.inner is itself a struct, not a bool/number scalar: correlating
// ld::Out1 (a BOOL) against d.inner exercises run_ld_tv_check's
// non-scalar-type-mismatch error, since neither side's typecast fallback
// (bool/number only) accepts a struct operand.
struct inner_t
{
  _Bool a;
  _Bool b;
};

struct ref_data
{
  _Bool in;
  struct inner_t inner;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.inner.a = d.in;
  d.inner.b = !d.in;
  return 0;
}
