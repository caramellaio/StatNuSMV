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
#include "nusmv/core/node/normalizers/MasterNormalizer.h"

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
#define STAT_ENV(self) (ENV_OBJECT(self)->environment)


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static StatVericationResult
  stat_problems_generator_verify_step(const StatProblemsGenerator_ptr self);

static StatTrace_ptr
  stat_problems_generator_simulate(StatProblemsGenerator_ptr self);

static StatVericationResult
  stat_problems_generator_verify_execution(const StatProblemsGenerator_ptr self,
                                           const StatTrace_ptr execution);

static Expr_ptr stat_problems_generator_gen_key(const NuSMVEnv_ptr env,
                                                const StatTrace_ptr exec);
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

  stat_problems_generator_deinit(self);
  stat_problems_generator_init(self, STAT_ENV(self));
  self->prop = property;
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
  env_object_init(ENV_OBJECT(self), env);

  self->executions_assoc = new_assoc();
  self->executions_list = Olist_create();

  self->prop = PROP(NULL);
  self->verification_method = STAT_INVALID_VERIFICATION;

  OVERRIDE(StatProblemsGenerator, gen_key) = stat_problems_generator_gen_key;
  OVERRIDE(StatProblemsGenerator, simulate) = stat_problems_generator_simulate;
}

void stat_problems_generator_deinit(StatProblemsGenerator_ptr self)
{
  Oiter oiter;

  env_object_deinit(ENV_OBJECT(self));

  OLIST_FOREACH(self->executions_list, oiter) {
    StatTrace_ptr execution = STAT_TRACE(Oiter_element(oiter));

    StatTrace_destroy(execution);
  }

  Olist_destroy(self->executions_list); self->executions_list = NULL;

  /* no need to destroy hash_ptr elements since they were destroyed before */
  free_assoc(self->executions_assoc); self->executions_assoc = NULL;

  self->verification_method = STAT_INVALID_VERIFICATION;
}
/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

static StatVericationResult
  stat_problems_generator_verify_step(const StatProblemsGenerator_ptr self)
{
  StatTrace_ptr new_exec = self->simulate(self);
  Expr_ptr new_exec_key = self->gen_key(STAT_ENV(self), new_exec);

  StatVericationResult res =
    stat_problems_generator_verify_execution(self, new_exec);

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

static StatVericationResult
  stat_problems_generator_verify_execution(const StatProblemsGenerator_ptr self,
                                           const StatTrace_ptr execution)
{
  error_unreachable_code_msg("Not yet implemented!\n");
}

static StatTrace_ptr
  stat_problems_generator_simulate(StatProblemsGenerator_ptr self)
{
  error_unreachable_code_msg("Not yet implemented!\n");
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

  retval = MasterNormalizer_normalize_node(master_norm, retval);

  return retval;
}
/**AutomaticEnd***************************************************************/
