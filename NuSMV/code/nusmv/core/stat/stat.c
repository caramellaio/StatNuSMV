/* ---------------------------------------------------------------------------


  This file is part of the ``simulate'' package of NuSMV version 2.
  Copyright (C) 1998-2001 by CMU and FBK-irst.

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
  \brief Statistical model checking routines

  This file contains functions for statistical model checking verification.

*/

#if HAVE_CONFIG_H
# include "nusmv-config.h"
#endif

#include "nusmv/core/parser/symbols.h"
#include "nusmv/core/stat/stat.h"
#include "nusmv/core/utils/StreamMgr.h"
#include "nusmv/core/utils/ErrorMgr.h"
#include "nusmv/core/utils/error.h" /* for CATCH(errmgr) */
#include "nusmv/core/stat/StatProblemsGenerator.h"

#include <math.h>
/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef boolean (*RandomVariable)(const NuSMVEnv_ptr, const Prop_ptr);
/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/* Single simulation verification functions */
static boolean verify_single_simulation(const NuSMVEnv_ptr env,
                                        const Prop_ptr ltl_prop,
                                        const char* env_stat_prob_gen);

static boolean verify_single_simulation_ltl(const NuSMVEnv_ptr env,
                                            const Prop_ptr ltl_prop);

static boolean verify_single_simulation_bmc(const NuSMVEnv_ptr env,
                                            const Prop_ptr ltl_prop);
/* Statistical algorithm functions */
static double Stat_approximate(const NuSMVEnv_ptr env,
                               const Prop_ptr prop_problem,
                               const double epsilon,
                               const double delta,
                               const RandomVariable Z);

static double Stat_stopping_rule_alg(const NuSMVEnv_ptr env,
                                     const Prop_ptr prob_prop,
                                     const double epsilon,
                                     const double delta,
                                     const RandomVariable Z);

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
int Stat_check_ltlspec(const NuSMVEnv_ptr env,
                       const Prop_ptr prop,
                       const double error_level,
                       const double confidence_level,
                       double *res)
{
  *res = Stat_approximate(env, prop, error_level, confidence_level,
                          &verify_single_simulation_ltl);

  return 0;
}

int Stat_check_ltlspec_bmc(const NuSMVEnv_ptr env,
                           const Prop_ptr prop,
                           const double error_level,
                           const double confidence_level,
                           double *res)
{
  *res = Stat_approximate(env, prop, error_level, confidence_level,
                          &verify_single_simulation_bmc);

  return 0;
}
/*---------------------------------------------------------------------------*/
/* Static function declaration                                               */
/*---------------------------------------------------------------------------*/
static double Stat_approximate(const NuSMVEnv_ptr env,
                               const Prop_ptr prop_problem,
                               const double epsilon,
                               const double delta,
                               const RandomVariable Z)
{
  double upsilon, upsilon2, mu_hat_Z;
  double rho_hat_Z;

  int N, alt, cnt;
  int i, successes;

  upsilon = 4 * (M_E - 2) * log(2 / delta) / pow(epsilon, 2);
  upsilon2 = 2 * (1 + sqrt(epsilon)) * (1 + 2*sqrt(epsilon)) *
             ((1 + log(1.5)) / log(2 / delta)) * upsilon;

  /* phase 1 */
  mu_hat_Z = Stat_stopping_rule_alg(env, prop_problem,
                                    min(0.5, sqrt(epsilon)),
                                    delta / 3,
                                    Z);

  /* phase 2 */
  N = ceil(2 * upsilon2 * epsilon / mu_hat_Z);
  alt = 0;
  cnt = 0;

  while (cnt < N) {
    boolean z, z_prime;

    cnt++;
    z = Z(env, prop_problem);

    cnt++;
    z_prime = Z(env, prop_problem);

    alt += z != z_prime;
  }

  /* phase 3 */

  rho_hat_Z = max(alt/ N, epsilon * pow(mu_hat_Z, 2));
  N = upsilon2 * rho_hat_Z / pow(mu_hat_Z, 2);
  successes = 0;

  for (i= 0; i < N; i++) {
    successes += Z(env, prop_problem);
  }

  mu_hat_Z = successes * 1.0 / N;

  return mu_hat_Z;
}

static double Stat_stopping_rule_alg(const NuSMVEnv_ptr env,
                                     const Prop_ptr prob_prop,
                                     const double epsilon,
                                     const double delta,
                                     const RandomVariable Z)
{
  double upsilon, upsilon1;
  double mu_hat_Z;
  int cnt, successes;

  upsilon = 4 * (M_E - 2) * log(2 / delta) / pow(epsilon, 2);
  upsilon1 = 1 + (1 + epsilon) * upsilon;

  cnt = 0;
  successes = 0;

  /* TODO[AB]: Handle case where cnt >>> successes (!!!) */
  while(successes < upsilon1) {
    cnt++;

    successes = successes + Z(env, prob_prop);
  }

  mu_hat_Z = successes * 1.0 / cnt;

  return mu_hat_Z;
}

static boolean verify_single_simulation_ltl(const NuSMVEnv_ptr env,
                                            const Prop_ptr ltl_prop)
{
  return verify_single_simulation(env, ltl_prop, ENV_STAT_PROB_GEN);
}

static boolean verify_single_simulation_bmc(const NuSMVEnv_ptr env,
                                            const Prop_ptr ltl_prop)
{
  return verify_single_simulation(env, ltl_prop, ENV_STAT_PROB_GEN_BMC);
}

static boolean verify_single_simulation(const NuSMVEnv_ptr env,
                                        const Prop_ptr ltl_prop,
                                        const char* env_stat_prob_gen)
{
  boolean retval;
  StatVericationResult res;

  const ErrorMgr_ptr errmgr =
    ERROR_MGR(NuSMVEnv_get_value(env, ENV_ERROR_MANAGER));

  StatProblemsGenerator_ptr prob_gen =
    STAT_PROBLEMS_GENERATOR(NuSMVEnv_get_value(env, env_stat_prob_gen));

  /* The property is prepared if it was not done before */
  if (ltl_prop != StatProblemsGenerator_get_prop(prob_gen)) {
    StatProblemsGenerator_prepare_property(prob_gen, ltl_prop);
  }

  res = StatProblemsGenerator_verify_step(prob_gen);

  switch (res) {
    case STAT_OK:
      retval = true;
      break;
    case STAT_NOT_OK:
      retval = false;
      break;

    default:
      ErrorMgr_nusmv_exit(errmgr, 1);
  }

  return retval;
}
