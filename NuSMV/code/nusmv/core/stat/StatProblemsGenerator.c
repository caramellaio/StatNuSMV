/* ---------------------------------------------------------------------------


  This file is part of the ``simulate'' package of NuSMV version 2.
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

static node_ptr stat_problems_generator_gen_key(const StatTrace_ptr exec);
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

  OVERRIDE(StatProblemsGenerator, gen_key) = stat_problems_generator_gen_key;
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
}
/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

static node_ptr stat_problems_generator_gen_key(const StatTrace_ptr exec)
{
  error_unreachable_code_msg("Not yet implemented!");
  return Nil;
}
/**AutomaticEnd***************************************************************/
