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

/* Used to perform random simulation */
#include "nusmv/core/simulate/simulate.h"

/* Trace are required to collect steps from the simulation */
#include "nusmv/core/trace/Trace.h"
#include "nusmv/core/trace/pkg_trace.h"

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
/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

StatProblemsGeneratorBmc_ptr StatProblemsGeneratorBmc_create(const NuSMVEnv_ptr env,
                                                             const int k)
{
  StatProblemsGeneratorBmc_ptr self = ALLOC(StatProblemsGeneratorBmc, 1);

  stat_problems_generator_bmc_init(self, env, k);

  return self;
}

void StatProblemsGeneratorBmc_destroy(StatProblemsGeneratorBmc_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  stat_problems_generator_bmc_deinit(self);

  FREE(self);
}
/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
void stat_problems_generator_bmc_init(StatProblemsGeneratorBmc_ptr self,
                                      const NuSMVEnv_ptr env,
                                      const int k)
{
  stat_problems_generator_init(STAT_PROBLEMS_GENERATOR(self), env);

  /* use fixed k verification method. */
  StatProblemsGenerator_set_verification_method(STAT_PROBLEMS_GENERATOR(self),
                                                STAT_BMC_VERIFICATION_FIXED_K);

  /* use bmc simulation */
  OVERRIDE(StatProblemsGenerator, simulate) =
    stat_problems_generator_bmc_simulate;

  self->k = k;
}

void stat_problems_generator_bmc_deinit(StatProblemsGeneratorBmc_ptr self)
{
  stat_problems_generator_deinit(STAT_PROBLEMS_GENERATOR(self));

  self->k = -1;
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
    int status = Simulate_simulate(env, false, Random, self_bmc->k, 0,
                                   false, false, bdd_true(dd));

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

    nusmv_assert(StatTrace_get_length(exec) == self_bmc->k);

    StatTrace_force_loopback(exec, self_bmc->k);

    nusmv_assert(StatTrace_is_generated(exec));
  }
  else {
    error_unreachable_code_msg("Error handling not yet implemented!!!\n");
  }

  return exec;
}

/**AutomaticEnd***************************************************************/


