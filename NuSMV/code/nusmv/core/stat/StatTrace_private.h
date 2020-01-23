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
  \brief Private and protected interface of class 'StatProblemsGenerator'

  This file can be included only by derived and friend classes

*/



#ifndef __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_PRIVATE_H__
#define __NUSMV_CORE_STAT_STAT_PROBLEM_GENERATOR_PRIVATE_H__

/* include public header */
#include "nusmv/core/stat/StatTrace.h"

typedef struct StatTrace_TAG {

  /* -------------------------------------------------- */
  /*                  Private members                   */
  /* -------------------------------------------------- */
  Trace_ptr generator_trace;

  unsigned int loopback;
  NodeList_ptr sexp_state_list;
  hash_ptr sexp_state_assoc;

  boolean generated;
} StatTrace;

/* ---------------------------------------------------------------------- */
/* Private methods to be used by derivated and friend classes only         */
/* ---------------------------------------------------------------------- */

void stat_trace_init(StatTrace_ptr self,
                     const Trace_ptr gen_trace);

void stat_trace_deinit(StatTrace_ptr self);
#endif /* __NUSMV_CORE_STAT_STAT_TRACE_PRIVATE_H__ */
