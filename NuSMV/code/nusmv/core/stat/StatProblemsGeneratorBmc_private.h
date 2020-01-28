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
  \brief Private and protected interface of class 'StatProblemsGeneratorBmc'

  This file can be included only by derived and friend classes

*/



#ifndef __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_BMC_PRIVATE_H__
#define __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_BMC_PRIVATE_H__
/* parent class */
#include "nusmv/core/stat/StatProblemsGenerator.h"
#include "nusmv/core/stat/StatProblemsGenerator_private.h"

/* Include used types */
#include "nusmv/core/utils/Olist.h"
#include "nusmv/core/prop/Prop.h"

/* include public header */
#include "nusmv/core/stat/StatProblemsGeneratorBmc.h"

typedef struct StatProblemsGeneratorBmc_TAG
{
  /* this MUST stay on the top */
  INHERITS_FROM(StatProblemsGenerator);

  /* -------------------------------------------------- */
  /*                  Private members                   */
  /* -------------------------------------------------- */
  /* fixed length for the simulation */
  int k;

  /* -------------------------------------------------- */
  /*                  Virtual methods                   */
  /* -------------------------------------------------- */

} StatProblemsGeneratorBmc;

/* ---------------------------------------------------------------------- */
/* Private methods to be used by derivated and friend classes only         */
/* ---------------------------------------------------------------------- */

void stat_problems_generator_bmc_init(StatProblemsGeneratorBmc_ptr self,
                                      const NuSMVEnv_ptr env);

void stat_problems_generator_bmc_deinit(StatProblemsGeneratorBmc_ptr self);
#endif /* __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_PRIVATE_H__ */

