#pragma once

#include <goto-programs/goto_functions.h>
#include <util/config/options.h>
#include <util/symtab/context.h>

/** WS1 item 4 (ws1/tv_harness_design.md): one-step translation validation
 *  between the LD front end's scan function and a reference C
 *  implementation of the same program, both present in `goto_functions`
 *  after a combined invocation with --secondary-entry-point.
 *
 *  Symexes __ESBMC_main (the LD side, unmodified) and, separately,
 *  __ESBMC_secondary_main (the reference side, via a temporary function-body
 *  swap so the existing __ESBMC_main-only symex entry point can reach it)
 *  each for one scan, asserts every correlated variable's post-scan value
 *  agrees in one shared solver, and checks UNSAT. Returns the process exit
 *  code (0 on a clean run, regardless of UNSAT/SAT, matching --ts-check's
 *  own convention of reporting a result rather than erroring on one).
 *
 *  Covers computed outputs only (§3 step 3 of the design doc), not free
 *  inputs (§3 step 2, not yet implemented): the two symex passes draw
 *  independent nondet values for a program's inputs (ESBMC's built-in
 *  side_effect2t nondet on the LD side vs. a nondet_bool()/nondet_int()
 *  call's return-value temporary on the MATIEC-C side), so a correlation
 *  file that lists an input variable is checking that variable against an
 *  unconstrained, unrelated free value on each side and will spuriously
 *  report SAT. List only variables the scan computes from its inputs.
 */
int run_ld_tv_check(
  contextt &context,
  optionst &options,
  goto_functionst &goto_functions);
