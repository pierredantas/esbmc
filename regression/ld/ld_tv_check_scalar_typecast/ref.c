// Same NotGate correlation as ld_tv_check_not_unsat, but d.out is declared
// unsigned char (MATIEC's own IEC BOOL representation, iec_types.h) rather
// than _Bool: ld::Out1 is ESBMC's native bool_t, so comparing the two
// correlated values exercises run_ld_tv_check's scalar-typecast fallback
// (both sides are bool/number scalars, so it typecasts rather than
// rejecting the pair as non-scalar-incompatible) instead of the identical-
// type path ld_tv_check_not_unsat covers.
struct ref_data
{
  unsigned char in;
  unsigned char out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool() ? 1 : 0;
  d.out = d.in ? 0 : 1;
  return 0;
}
