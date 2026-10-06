// d has no "nosuchfield" member, and d.inner is a plain nested struct (no
// pointer indirection, unlike the inner_ptr case this file once used):
// correlating ld::Out1 against d.inner.nope exercises
// project_field_path's member-name lookup-miss return on the *inner*
// struct (ld_tv_check.cpp), after the "inner" lookup on d itself succeeds.
struct inner_t
{
  _Bool nosuchfield;
};

struct ref_data
{
  _Bool in;
  _Bool out;
  struct inner_t inner;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.out = !d.in;
  d.inner.nosuchfield = d.in;
  return 0;
}
