// d.inner_ptr is a pointer to a struct: correlating ld::Out1 against
// d.inner_ptr.nosuchfield exercises project_field_path's pointer-subtype
// unwrap (ld_tv_check.cpp) on the "inner_ptr" lookup step, then its
// is_struct_type check on the unwrapped pointee for the next step
// ("nosuchfield"); whichever of the two actually rejects the chain here,
// this fixture's purpose is the unwrap branch itself, not a specific
// field-name outcome.
struct inner_t
{
  _Bool nosuchfield;
};

struct ref_data
{
  _Bool in;
  _Bool out;
  struct inner_t *inner_ptr;
};

int ref_scan(void)
{
  struct inner_t inner = {0};
  struct ref_data d = {0};
  d.inner_ptr = &inner;
  d.in = nondet_bool();
  d.out = !d.in;
  return 0;
}
