/* ---------------------------------------------------------------------------


  This file is part of the ``stat'' package of NuSMV version 2.
  Copyright (C) 2012 by FBK-irst.

  NuSMV version 2 is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information on NuSMV see <http://nusmv.fbk.eu>
  or email to <nusmv-users@fbk.eu>.
  Please report bugs to <nusmv-users@fbk.eu>.

  To contact the NuSMV development board, email to <nusmv@fbk.eu>.

-----------------------------------------------------------------------------*/

/*!
  \author Alberto Bombardelli
  \brief Implementation of class 'StatProblemsGeneratorBmc'

*/

#include "nusmv/core/stat/StatProblemsGeneratorBmc.h"
#include "nusmv/core/stat/StatProblemsGeneratorBmc_private.h"

/* stat sexp problem generation */
#include "nusmv/core/stat/statInt.h"

/* Used to perform random simulation */
#include "nusmv/core/simulate/simulate.h"

/* Trace are required to collect steps from the simulation */
#include "nusmv/core/trace/Trace.h"
#include "nusmv/core/trace/pkg_trace.h"

/* used for bmc verification */
#include "nusmv/core/bmc/bmcDump.h"
#include "nusmv/core/bmc/bmcBmc.h"
#include "nusmv/core/bmc/bmc.h"
/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static StatTrace_ptr
  stat_problems_generator_bmc_simulate(StatProblemsGenerator_ptr self);

static StatVericationResult
  stat_problems_generator_bmc_verify_execution(const StatProblemsGenerator_ptr gen,
                                               const StatTrace_ptr execution);

static Expr_ptr dummy_gen_key(const NuSMVEnv_ptr env,
                              const StatTrace_ptr exec);
static inline int get_opt_loop(const StatProblemsGeneratorBmc_ptr self);
static inline int get_opt_k(const StatProblemsGeneratorBmc_ptr self);
/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

StatProblemsGeneratorBmc_ptr StatProblemsGeneratorBmc_create(const NuSMVEnv_ptr env)
{
  StatProblemsGeneratorBmc_ptr self = ALLOC(StatProblemsGeneratorBmc, 1);

  stat_problems_generator_bmc_init(self, env);

  return self;
}

void StatProblemsGeneratorBmc_destroy(StatProblemsGeneratorBmc_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  stat_problems_generator_bmc_deinit(self);

  FREE(self);
}

int StatProblemsGeneratorBmc_get_k(const StatProblemsGeneratorBmc_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  return get_opt_k(self);
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
void stat_problems_generator_bmc_init(StatProblemsGeneratorBmc_ptr self,
                                      const NuSMVEnv_ptr env)
{
  stat_problems_generator_init(STAT_PROBLEMS_GENERATOR(self), env);

  /* use fixed k verification method. */
  StatProblemsGenerator_set_verification_method(STAT_PROBLEMS_GENERATOR(self),
                                                STAT_BMC_VERIFICATION_FIXED_K);

  /* use fixed simulation */
  OVERRIDE(StatProblemsGenerator, simulate) =
    stat_problems_generator_bmc_simulate;

  /* use bmc problem generator */
  OVERRIDE(StatProblemsGenerator, verify_execution) =
    stat_problems_generator_bmc_verify_execution;

  /* this function returns a different value on each execution */
  OVERRIDE(StatProblemsGenerator, gen_key) = dummy_gen_key;
}

void stat_problems_generator_bmc_deinit(StatProblemsGeneratorBmc_ptr self)
{
  stat_problems_generator_deinit(STAT_PROBLEMS_GENERATOR(self));
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

static StatTrace_ptr
  stat_problems_generator_bmc_simulate(StatProblemsGenerator_ptr self)
{
  const NuSMVEnv_ptr env = EnvObject_get_environment(ENV_OBJECT(self));

  const BddEnc_ptr enc = BDD_ENC(NuSMVEnv_get_value(env, ENV_BDD_ENCODER));
  const TraceMgr_ptr tm =
    TRACE_MGR(NuSMVEnv_get_value(env, ENV_TRACE_MGR));

  DDMgr_ptr dd = BddEnc_get_dd_manager(enc);

  StatProblemsGeneratorBmc_ptr self_bmc = STAT_PROBLEMS_GENERATOR_BMC(self);
  StatTrace_ptr exec = StatTrace_create();

  /* pick the initial state */
  if (0 == Simulate_pick_state(env, TRACE_LABEL_INVALID, Random,
                               0, false, NULL)) {
    TraceIter trace_iter;

    const int trace_id = TraceMgr_get_current_trace_number(tm);
    const Trace_ptr curr_trace = TraceMgr_get_trace_at_index(tm, trace_id);

    /* launch random simulation of length k */
    int status = Simulate_simulate(env, false, Random, get_opt_k(self_bmc),
                                   0, false, false, bdd_true(dd));

    if (0 != status) {
      error_unreachable_code_msg("Error handling not yet implemented!!!\n");
    }

    TRACE_FOREACH(curr_trace, trace_iter) {
      Expr_ptr state_sexp = TraceUtils_fetch_as_sexp(curr_trace, trace_iter,
                                                     TRACE_ITER_ALL_VARS);
      /* loopback is not allowed:
         We force it at the end of the states computation
      */
      StatTrace_add_state(exec, state_sexp, false);
    }

    /* k steps + initial state */
    nusmv_assert(StatTrace_get_length(exec) == get_opt_k(self_bmc) + 1);

    StatTrace_force_loopback(exec, get_opt_k(self_bmc));

    nusmv_assert(StatTrace_is_generated(exec));
  }
  else {
    error_unreachable_code_msg("Error handling not yet implemented!!!\n");
  }

  return exec;
}

static StatVericationResult
  stat_problems_generator_bmc_verify_execution(const StatProblemsGenerator_ptr gen,
                                               const StatTrace_ptr execution)
{
  Prop_ptr gen_prop;
  StatVericationResult retval;
  int res;
  int relative_loop;

  const NuSMVEnv_ptr env = EnvObject_get_environment(ENV_OBJECT(gen));
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));
  const NodeMgr_ptr nodemgr = NODE_MGR(NuSMVEnv_get_value(env, ENV_NODE_MGR));

  const Prop_ptr prop = StatProblemsGenerator_get_prop(gen);
  const StatProblemsGeneratorBmc_ptr self = STAT_PROBLEMS_GENERATOR_BMC(gen);

  /* k steps + initial state */
  nusmv_assert(StatTrace_get_length(execution) == get_opt_k(self) + 1);

  gen_prop = StatSexpProblem_gen_bmc_problem(env, execution, prop);

  nusmv_assert(STAT_BMC_VERIFICATION_FIXED_K == gen->verification_method);

  retval = STAT_NOT_VERIFIED;

  /* Call BMC internal functions... */
  res = Bmc_GenSolveLtl(env, gen_prop, get_opt_k(self), get_opt_loop(self),
                        true, true, BMC_DUMP_NONE, NULL);

  if (0 != res) {
    retval = STAT_INTERNAL_ERROR;
  }
  else if (0 == Prop_get_trace(gen_prop)) {
    retval = STAT_OK;
  }
  else {
    retval = STAT_NOT_OK;
  }

  Prop_destroy(gen_prop);

  return retval;
}

static inline int get_opt_loop(const StatProblemsGeneratorBmc_ptr self)
{
  const NuSMVEnv_ptr env = EnvObject_get_environment(ENV_OBJECT(self));
  const OptsHandler_ptr opts =
    OPTS_HANDLER(NuSMVEnv_get_value(env, ENV_OPTS_HANDLER));

  const char* pb_loop = get_bmc_pb_loop(opts);

  return Bmc_Utils_ConvertLoopFromString(pb_loop, NULL);
}

static inline int get_opt_k(const StatProblemsGeneratorBmc_ptr self)
{
  const NuSMVEnv_ptr env = EnvObject_get_environment(ENV_OBJECT(self));
  const OptsHandler_ptr opts =
    OPTS_HANDLER(NuSMVEnv_get_value(env, ENV_OPTS_HANDLER));

  return get_bmc_pb_length(opts);
}

static Expr_ptr dummy_gen_key(const NuSMVEnv_ptr env,
                              const StatTrace_ptr exec)
{
  static int counter = 0;
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));

  counter++;

  /* TODO[AB]: using a counter is not a good idea */
  return ExprMgr_number(exprs, counter);
}
/**AutomaticEnd***************************************************************/


