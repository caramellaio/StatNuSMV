/* ---------------------------------------------------------------------------


  This file is part of the ``stat'' package of NuSMV version 2.
  Copyright (C) 2000-2001 by FBK-irst and University of Trento.

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
  \brief The header file for the <tt>cmd</tt> module, the user
  commands handling layer.

  \todo: Missing description

*/


#ifndef __NUSMV_SHELL_STAT_STAT_CMD_H__
#define __NUSMV_SHELL_STAT_STAT_CMD_H__

# include "nusmv-config.h"

#include "cudd/util.h"
#include "nusmv/core/utils/utils.h"
#include "nusmv/core/cinit/NuSMVEnv.h"

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

void Stat_Cmd_init(NuSMVEnv_ptr env);

void Stat_Cmd_quit(NuSMVEnv_ptr env);

int Stat_CommandCheckLtlspec(NuSMVEnv_ptr env, int argc, char** argv);

int Stat_CommandCheckLtlspecBmc(NuSMVEnv_ptr env, int argc, char** argv);
/**AutomaticEnd***************************************************************/

#endif /* __NUSMV_SHELL_BMC_BMC_CMD_H__ */
