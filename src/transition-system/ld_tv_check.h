#pragma once

#include <goto-programs/goto_functions.h>
#include <util/config/options.h>
#include <util/symtab/context.h>

/** WS1 item 4 (ws1/tv_harness_design.md): one-step translation validation
 *  between the LD front end's scan function and a reference C
 *  implementation of the same program, both present in `goto_functions`
 *  after a combined invocation with --secondary-entry-point.
 *
 *  Symexes __ESBMC_main (the LD side) and, separately,
 *  __ESBMC_secondary_main (the reference side, via a temporary function-body
 *  swap so the existing __ESBMC_main-only symex entry point can reach it)
 *  each for one scan, asserts the correlation file's equalities in one
 *  shared solver, and checks whether any out or state variable can differ
 *  afterwards. Returns the process exit code (0 on a clean run, regardless
 *  of UNSAT/SAT, matching --ts-check's own convention).
 *
 *  Correlation entries are "[in|out|state] ld_var ref.field.path". The two
 *  passes share no symbols: the reference's globals and nondets are
 *  renamed, so an input or a pre-state is common to both sides only when an
 *  in or state entry says so. State not listed starts at its initial value
 *  on both sides. Assumes are not applied, and the LD side's own property
 *  assertions are dropped.
 */
int run_ld_tv_check(
  contextt &context,
  optionst &options,
  goto_functionst &goto_functions);
