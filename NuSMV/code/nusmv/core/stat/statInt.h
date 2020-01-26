 /* ---------------------------------------------------------------------------


  This file is part of the ``stat'' package of NuSMV version 2.
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
  \brief Internal Header File for the stat package

  Internal Header File for the stat package

*/


#ifndef __NUSMV_CORE_STAT_STAT_INT_H__
#define __NUSMV_CORE_STAT_STAT_INT_H__

#include "nusmv/core/stat/StatTrace.h"
#include "nusmv/core/prop/Prop.h"
/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

SymbLayer_ptr StatSexpProblem_prepare_layer(const NuSMVEnv_ptr env,
                                            SymbTable_ptr symb_table,
                                            const Expr_ptr counter_var,
                                            const int k);

void StatSexpProblem_destroy_layer(const NuSMVEnv_ptr env,
                                   SymbTable_ptr symb_table,
                                   SymbLayer_ptr layer);

Prop_ptr StatSexpProblem_gen_problem(const NuSMVEnv_ptr env,
                                     const StatTrace_ptr stat_trace,
                                     const Expr_ptr counter_var,
                                     const Prop_ptr prop);

Prop_ptr StatSexpProblem_gen_single_state_problem(const NuSMVEnv_ptr env,
                                                  const StatTrace_ptr execution,
                                                  const Prop_ptr prop);

#endif /* __NUSMV_CORE_STAT_STAT_INT_H__ */
