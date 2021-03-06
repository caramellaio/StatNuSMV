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
  \brief Implementation of class 'StatProblemsGenerator'

*/

#include "nusmv/core/stat/StatProblemsGenerator.h"
#include "nusmv/core/stat/StatProblemsGenerator_private.h"
#include "nusmv/core/stat/statInt.h"

/* Used to generate the key. */
#include "nusmv/core/node/normalizers/MasterNormalizer.h"

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

#define STAT_COUNTER_VAR_NAME "__GENERATED_COUNTER_VAR__"

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define STAT_ENV(self) (ENV_OBJECT(self)->environment)


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static StatTrace_ptr
  stat_problems_generator_simulate(StatProblemsGenerator_ptr self);

static StatVericationResult
  stat_problems_generator_verify_execution(const StatProblemsGenerator_ptr self,
                                           const StatTrace_ptr execution);

static Expr_ptr stat_problems_generator_gen_key(const NuSMVEnv_ptr env,
                                                const StatTrace_ptr exec);

static Expr_ptr dummy_gen_key(const NuSMVEnv_ptr env,
                              const StatTrace_ptr exec);
static void deinit_executions(StatProblemsGenerator_ptr self);
/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

StatProblemsGenerator_ptr StatProblemsGenerator_create(const NuSMVEnv_ptr env)
{
  StatProblemsGenerator_ptr self = ALLOC(StatProblemsGenerator, 1);

  stat_problems_generator_init(self, env);

  return self;
}

void StatProblemsGenerator_destroy(StatProblemsGenerator_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  stat_problems_generator_deinit(self);

  FREE(self);
}

void StatProblemsGenerator_prepare_property(StatProblemsGenerator_ptr self,
                                            const Prop_ptr property)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  deinit_executions(self);
  /* We do not call init since it overrides simulate and verify... */
  self->executions_assoc = new_assoc();
  self->executions_list = Olist_create();

  self->prop = property;
}

Prop_ptr StatProblemsGenerator_get_prop(const StatProblemsGenerator_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  return self->prop;
}

StatVericationResult StatProblemsGenerator_verify_step(StatProblemsGenerator_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  /* A property should have been selected */
  PROP_CHECK_INSTANCE(self->prop);

  return stat_problems_generator_verify_step(self);
}

StatVerificationMethod
  StatProblemsGenerator_get_verification_method(const StatProblemsGenerator_ptr self)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  return self->verification_method;
}

void StatProblemsGenerator_set_verification_method(StatProblemsGenerator_ptr self,
                                                   StatVerificationMethod method)
{
  STAT_PROBLEMS_GENERATOR_CHECK_INSTANCE(self);

  self->verification_method = method;
}
/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
void stat_problems_generator_init(StatProblemsGenerator_ptr self,
                                  const NuSMVEnv_ptr env)
{
  SymbTable_ptr st = SYMB_TABLE(NuSMVEnv_get_value(env, ENV_SYMB_TABLE));

  env_object_init(ENV_OBJECT(self), env);

  self->executions_assoc = new_assoc();
  self->executions_list = Olist_create();

  self->prop = PROP(NULL);
  self->verification_method = STAT_LTL_VERIFICATION;
  self->counter_var =
    SymbTable_get_fresh_symbol_name(st, STAT_COUNTER_VAR_NAME);

  OVERRIDE(StatProblemsGenerator, gen_key) = stat_problems_generator_gen_key;
  OVERRIDE(StatProblemsGenerator, gen_key) = dummy_gen_key;
  OVERRIDE(StatProblemsGenerator, simulate) = stat_problems_generator_simulate;
  OVERRIDE(StatProblemsGenerator, verify_execution) = stat_problems_generator_verify_execution;
}

void stat_problems_generator_deinit(StatProblemsGenerator_ptr self)
{
  env_object_deinit(ENV_OBJECT(self));

  deinit_executions(self);

  self->counter_var = Nil;
  self->verification_method = STAT_INVALID_VERIFICATION;
}

StatVericationResult
  stat_problems_generator_verify_step(const StatProblemsGenerator_ptr self)
{
  StatTrace_ptr new_exec = self->simulate(self);
  Expr_ptr new_exec_key = self->gen_key(STAT_ENV(self), new_exec);

  StatVericationResult res = self->verify_execution(self, new_exec);

  /* We assume that it is not possible to verify twice the same execution */
  nusmv_assert(STAT_NOT_VERIFIED ==
               (StatVericationResult)find_assoc(self->executions_assoc,
                                                new_exec_key));

  /* Store the result */
  insert_assoc(self->executions_assoc, new_exec_key, NODE_FROM_INT(res));

  /* Add the execution to the list */
  Olist_append(self->executions_list, (void*)new_exec);

  return res;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


static StatVericationResult
  stat_problems_generator_verify_execution(const StatProblemsGenerator_ptr self,
                                           const StatTrace_ptr execution)
{
  StatVericationResult res = STAT_INTERNAL_ERROR;
  const NuSMVEnv_ptr env = STAT_ENV(self);
  const StreamMgr_ptr streams = STREAM_MGR(NuSMVEnv_get_value(env, ENV_STREAM_MANAGER));
  SymbTable_ptr symb_table = SYMB_TABLE(NuSMVEnv_get_value(env, ENV_SYMB_TABLE));

  Prop_ptr to_verify = NULL;
  SymbLayer_ptr layer = NULL;
  int k = StatTrace_get_length(execution);

  FILE * f = StreamMgr_reset_output_stream(streams);
  nusmv_assert(0 < k);


  /* case the execution is a single state loop */
  if (1 == k) {
    to_verify =
      StatSexpProblem_gen_single_state_problem(env, execution, self->prop);
  }
  else {
    /* multiple state verification requires counter var */
    layer =
      StatSexpProblem_prepare_layer(env, symb_table, self->counter_var, k);

    to_verify =
      StatSexpProblem_gen_problem(env, execution, self->counter_var, self->prop);
  }

  if (STAT_LTL_VERIFICATION == self->verification_method) {
    Prop_verify(to_verify);

    switch(Prop_get_status(to_verify)) {
      case Prop_True:
        res = STAT_OK;
        break;
      case Prop_False:
        res = STAT_NOT_OK;
        break;

      default:
        res = STAT_INTERNAL_ERROR;
        break;
    }
  }
  else {
    error_unreachable_code_msg("Not yet implemented!\n");
  }

  if (NULL != layer) {
    StatSexpProblem_destroy_layer(env, symb_table, layer);
    layer = NULL;
  }

  StreamMgr_set_output_stream(streams, f);
  return res;
}


static StatTrace_ptr
  stat_problems_generator_simulate(StatProblemsGenerator_ptr self)
{
  /* TODO[AB]: This function prints undesired output, fix it */
  const NuSMVEnv_ptr env = STAT_ENV(self);
  const BddEnc_ptr enc = BDD_ENC(NuSMVEnv_get_value(env, ENV_BDD_ENCODER));
  const TraceMgr_ptr tm =
    TRACE_MGR(NuSMVEnv_get_value(env, ENV_TRACE_MGR));
  const MasterNormalizer_ptr master_norm =
    MASTER_NORMALIZER(NuSMVEnv_get_value(env, ENV_NODE_NORMALIZER));

  DDMgr_ptr dd = BddEnc_get_dd_manager(enc);

  StatTrace_ptr exec = StatTrace_create();

  if (0 == Simulate_pick_state(env, TRACE_LABEL_INVALID, Random,
                               0, false, NULL)) {
    boolean found = false;
    const int trace_id = TraceMgr_get_current_trace_number(tm);
    const Trace_ptr curr_trace = TraceMgr_get_trace_at_index(tm, trace_id);

    /* (0) Encode initial state */
    Expr_ptr init_sexp =
      TraceUtils_fetch_as_sexp(curr_trace, Trace_first_iter(curr_trace),
                               TRACE_ITER_SF_VARS);

    init_sexp = MasterNormalizer_normalize_node(master_norm, init_sexp);

    /* No loopback in the first state */
    StatTrace_add_state(exec, init_sexp, false);

    while (! found) {
      Expr_ptr state_sexp = Nil;
      boolean allow_loopback = true;

      /* (1.1) Launch single step simulation */
      int status = Simulate_simulate(env, false, Random, 1, 0,
                                     false, false, bdd_true(dd));

      if (0 != status) {
        error_unreachable_code_msg("Error handling not yet implemented!!!\n");
      }

      /* (1.2) Collect state sexp */
      state_sexp =
        TraceUtils_fetch_as_sexp(curr_trace,
                                 Trace_last_iter(curr_trace),
                                 TRACE_ITER_ALL_VARS);
      state_sexp = MasterNormalizer_normalize_node(master_norm, state_sexp);

      /* (1.3) Add state sexp to StatTrace */
      StatTrace_add_state(exec, state_sexp, true);

      /* case we find an already visited state */
      if (StatTrace_is_generated(exec)) {
        /* Check if we already verified this execution.
           If so do not allow loopbacks
        */
        Expr_ptr new_key = self->gen_key(env, exec);

        /* we allow the loopback if this execution have not been verified yet */
        if (Nil != find_assoc(self->executions_assoc, new_key)) {
          StatTrace_remove_loopback(exec);
          StatTrace_add_state(exec, state_sexp, false);
        }
      }


      /* If this execution generated the loopback we can exit from the loop */
      if (StatTrace_is_generated(exec)) {
        nusmv_assert(allow_loopback);
        found = true;
      }
    }
  }
  else {
    error_unreachable_code_msg("Error handling not yet implemented!!!\n");
  }

  return exec;
}

/*!
  /brief Generates a key using states

  Note: state order and variable order is important !!!
*/
static Expr_ptr stat_problems_generator_gen_key(const NuSMVEnv_ptr env,
                                                const StatTrace_ptr exec)
{
  ListIter_ptr iter;
  NodeList_ptr state_list;
  Expr_ptr retval;

  const MasterNormalizer_ptr master_norm =
    MASTER_NORMALIZER(NuSMVEnv_get_value(env, ENV_NODE_NORMALIZER));
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));

  nusmv_assert(StatTrace_is_generated(exec));

  retval = ExprMgr_true(exprs);

  state_list = StatTrace_get_sexp_states(exec);

  NODE_LIST_FOREACH(state_list, iter) {
    Expr_ptr state_sexp = NodeList_get_elem_at(state_list, iter);

    retval = ExprMgr_and(exprs, state_sexp, retval);
  }

  retval =
    ExprMgr_and(exprs, ExprMgr_number(exprs, StatTrace_get_loopback(exec)), retval);
  retval = MasterNormalizer_normalize_node(master_norm, retval);

  return retval;
}

static void deinit_executions(StatProblemsGenerator_ptr self)
{
  Oiter oiter;

  OLIST_FOREACH(self->executions_list, oiter) {
    StatTrace_ptr execution = STAT_TRACE(Oiter_element(oiter));

    StatTrace_destroy(execution);
  }

  Olist_destroy(self->executions_list); self->executions_list = NULL;

  /* no need to destroy hash_ptr elements since they were destroyed before */
  free_assoc(self->executions_assoc); self->executions_assoc = NULL;
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
