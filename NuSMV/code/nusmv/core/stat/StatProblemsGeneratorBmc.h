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
  \brief Public interface of class 'StatProblemsGeneratorBmc'

  \todo: Missing description

*/



#ifndef __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_BMC_H__
#define __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_BMC_H__

#include "nusmv/core/stat/StatProblemsGenerator.h"
/*!
  \struct StatProblemsGenerator
  \brief Definition of the public accessor for class StatProblemsGenerator


*/
typedef struct StatProblemsGeneratorBmc_TAG*  StatProblemsGeneratorBmc_ptr;


/*!
  \brief To cast and check instances of class StatProblemsGenerator

  These macros must be used respectively to cast and to check
  instances of class StatProblemsGenerator
*/
#define STAT_PROBLEMS_GENERATOR_BMC(self) \
         ((StatProblemsGeneratorBmc_ptr) self)

/*!
  \brief \todo Missing synopsis

  \todo Missing description
*/
#define STAT_PROBLEMS_GENERATOR_BMC_CHECK_INSTANCE(self) \
         (nusmv_assert(STAT_PROBLEMS_GENERATOR_BMC(self) != STAT_PROBLEMS_GENERATOR_BMC(NULL)))

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/* Constructors ***************************************************************/

/*!
  \methodof StatProblemsGeneratorBmc
  \brief The StatProblemsGeneratorBmc class constructor

  The StatProblemsGenerator class constructor

  \sa StatProblemsGenerator_destroy
*/
StatProblemsGeneratorBmc_ptr StatProblemsGeneratorBmc_create(const NuSMVEnv_ptr env,
                                                             const int k);

/* Destructors ****************************************************************/

/*!
  \methodof StatProblemsGeneratorBmc
  \brief The StatProblemsGeneratorBmc class destructor

  The StatProblemsGeneratorBmc class destructor

  \sa StatProblemsGeneratorBmc_create
*/
void StatProblemsGeneratorBmc_destroy(StatProblemsGeneratorBmc_ptr self);
#endif /* __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_BMC_H__ */
