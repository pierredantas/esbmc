// A correct reference, unused for its logic here: this fixture targets the
// correlation-file parser's "ref.field.path" branch (ld_tv_check.cpp,
// read_correlation_file), not the symex/solve path, so program.ld never
// reaches a solver on this test.
struct ref_data
{
  _Bool in;
  _Bool out;
};

int ref_scan(void)
{
  struct ref_data d = {0};
  d.in = nondet_bool();
  d.out = !d.in;
  return 0;
}
