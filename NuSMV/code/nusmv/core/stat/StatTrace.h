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
  \brief Public interface of class 'StatTrace'

  \todo: Missing description

*/



#ifndef __NUSMV_CORE_STAT_STAT_TRACE_GENERATOR_H__
#define __NUSMV_CORE_STAT_STAT_TRACE_GENERATOR_H__

#include "nusmv/core/utils/NodeList.h"
#include "nusmv/core/wff/ExprMgr.h"
#include "nusmv/core/trace/Trace.h"
/*!
  \struct StatTrace
  \brief Definition of public accessor for class StatTrace

*/
typedef struct StatTrace_TAG* StatTrace_ptr;

/*!
  \brief To cast and check instances of class StatTrace

  These macros must be used respectively to cast and to check
  instances of class StatTrace
*/
#define STAT_TRACE(self) \
         ((StatTrace_ptr) self)

/*!
  \brief \todo Missing synopsis

  \todo Missing description
*/
#define STAT_TRACE_CHECK_INSTANCE(self) \
         (nusmv_assert(STAT_TRACE(self) != STAT_TRACE(NULL)))

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/* Constructors ***************************************************************/

/*!
  \methodof StatTrace
  \brief The StatTrace class constructor

  The StatTrace class constructor

  \sa StatTrace_destroy
*/
StatTrace_ptr StatTrace_create(void);

/* Destructors ****************************************************************/

/*!
  \methodof StatTrace
  \brief The StatTrace class destructor

  The StatTrace class destructor

  \sa StatTrace_create
*/
void StatTrace_destroy(StatTrace_ptr self);

/*!
  \methodof StatTrace
  \brief Returns true iff the state sexp is inside StatTrace

  Assumes that sexp_state is normalized
  \sa StatTrace_add_state
*/
boolean StatTrace_has_state(const StatTrace_ptr self,
                            const Expr_ptr sexp_state);

/*!
  \methodof StatTrace
  \brief Add a new state to StatTrace.


  Assumes that sexp_state is normalized

  If allow loopback is true and `sexp_state` is a visited state then
    mark the StatTrace as generated and compute the loopback

  \sa StatTrace_has_state
*/
void StatTrace_add_state(const StatTrace_ptr self,
                         const Expr_ptr state_sexp,
                         const boolean allow_loopback);

/*!
  \methodof StatTrace
  \brief Getter for generated

*/
boolean StatTrace_is_generated(const StatTrace_ptr self);

/*!
  \methodof StatTrace
  \brief Getter for loopback

*/
int StatTrace_get_loopback(const StatTrace_ptr self);

/*!
  \methodof StatTrace
  \brief Force loopback

  set self->loopback = loopback and set generated as true.
*/
void StatTrace_force_loopback(StatTrace_ptr self,
                              const int loopback);

/*!
  \methodof StatTrace
  \brief Remove the trace loopback and the generated flag

*/
void StatTrace_remove_loopback(const StatTrace_ptr self);

/*!
  \methodof StatTrace
  \brief Getter for trace length

*/
int StatTrace_get_length(const StatTrace_ptr self);

/*!
  \methodof StatTrace
  \brief Getter for sexp_states

  Note: Do not modify the list! Do not destroy the list!
*/
NodeList_ptr StatTrace_get_sexp_states(const StatTrace_ptr self);
#endif
