#include <esbmc/esbmc_parseoptions.h>

#include <transition-system/ld_tv_check.h>

int esbmc_parseoptionst::do_ld_tv_check(
  optionst &options,
  goto_functionst &goto_functions)
{
  return run_ld_tv_check(context, options, goto_functions);
}
