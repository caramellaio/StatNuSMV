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
E
  To contact the NuSMV development board, email to <nusmv@fbk.eu>.

-----------------------------------------------------------------------------*/

/*!
  \author Alberto Bombardelli
  \brief ...

*/


#ifndef __NUSMV_CORE_STAT_STAT_H__
#define __NUSMV_CORE_STAT_STAT_H__

#include "nusmv/core/utils/utils.h"
#include "nusmv/core/dd/dd.h"
#include "nusmv/core/fsm/bdd/BddFsm.h"
#include "nusmv/core/node/node.h"
#include "nusmv/core/trace/TraceMgr.h"
#include "nusmv/core/trace/TraceLabel.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define STAT_INTERNAL_COUNTER_NAME "__INTERNAL_COUNTER_"
/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#warning TODO[AB]: Add description
int Stat_check_ltlspec(const NuSMVEnv_ptr env,
                       const Prop_ptr prop,
                       const double error_level,
                       const double confidence_level,
                       double *res);

int Stat_check_ltlspec_bmc(const NuSMVEnv_ptr env,
                           const Prop_ptr prop,
                           const double error_level,
                           const double confidence_level,
                           double *res);
#endif /* __NUSMV_CORE_STAT_STAT_H__ */
