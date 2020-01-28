/* ---------------------------------------------------------------------------


  This file is part of the ``stat'' package.
  %COPYRIGHT%


-----------------------------------------------------------------------------*/

/*!
  \author Alberto Bombardelli
  \brief Package initializer and deinitializer

  \todo: Missing description

*/

#include "nusmv/core/stat/statInt.h"
#include "nusmv/core/stat/StatProblemsGenerator.h"
#include "nusmv/core/stat/StatProblemsGeneratorBmc.h"

#include "nusmv/core/cinit/NuSMVEnv.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
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

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

void Stat_Pkg_init(NuSMVEnv_ptr env)
{
  StatProblemsGenerator_ptr pg = StatProblemsGenerator_create(env);
  StatProblemsGeneratorBmc_ptr b_pg = StatProblemsGeneratorBmc_create(env);

  NuSMVEnv_set_value(env, ENV_STAT_PROB_GEN, (void*)pg);
  NuSMVEnv_set_value(env, ENV_STAT_PROB_GEN_BMC, (void*)b_pg);
  /* TODO[AB]: Add also the BMC generator... */
}

void Stat_Pkg_quit(NuSMVEnv_ptr env)
{
  StatProblemsGenerator_ptr pg =
    STAT_PROBLEMS_GENERATOR(NuSMVEnv_get_value(env, ENV_STAT_PROB_GEN));
  StatProblemsGeneratorBmc_ptr b_pg =
    STAT_PROBLEMS_GENERATOR_BMC(NuSMVEnv_get_value(env, ENV_STAT_PROB_GEN_BMC));

  StatProblemsGenerator_destroy(pg);
  StatProblemsGeneratorBmc_destroy(b_pg);

  NuSMVEnv_remove_value(env, ENV_STAT_PROB_GEN);
  NuSMVEnv_remove_value(env, ENV_STAT_PROB_GEN_BMC);
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



