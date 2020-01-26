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
  \brief Implementation of class 'StatTrace'

*/

#include "nusmv/core/stat/StatTrace.h"
#include "nusmv/core/stat/StatTrace_private.h"

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

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

StatTrace_ptr StatTrace_create(void)
{
  StatTrace_ptr self = ALLOC(StatTrace, 1);

  stat_trace_init(self);

  return self;
}

void StatTrace_destroy(StatTrace_ptr self)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  stat_trace_deinit(self);
}

boolean StatTrace_has_state(const StatTrace_ptr self,
                            const Expr_ptr sexp_state)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  return 0 != find_assoc(self->sexp_state_assoc, sexp_state);
}

void StatTrace_add_state(const StatTrace_ptr self,
                         const Expr_ptr state_sexp,
                         const boolean allow_loopback)
{
  /* use assoc to check if the state was there. ASSUMES state_sexp normalized */
  boolean add_new_state = true;

  STAT_TRACE_CHECK_INSTANCE(self);

  if (allow_loopback) {
    int loopback = NODE_TO_INT(find_assoc(self->sexp_state_assoc, state_sexp));
    if (0 != loopback) {
      nusmv_assert(0 < loopback);

      add_new_state = false;
      loopback--; /* 0 is a valid loopback but find_assoc does not allow 0 */

      /* update internal informations */
      self->loopback = (unsigned int)loopback;
      self->generated = true;
    }
  }

  if (add_new_state) {
    int state = NodeList_get_length(self->sexp_state_list);

    nusmv_assert(state >= 0);
    state++; /* + 1 is for find_assoc */

    insert_assoc(self->sexp_state_assoc, state_sexp, NODE_FROM_INT(state));
    NodeList_append(self->sexp_state_list, state_sexp);
  }
}

void StatTrace_remove_loopback(const StatTrace_ptr self)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  nusmv_assert(self->generated);

  self->loopback = -1;
  self->generated = false;
}

boolean StatTrace_is_generated(const StatTrace_ptr self)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  return self->generated;
}

NodeList_ptr StatTrace_get_sexp_states(const StatTrace_ptr self)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  return self->sexp_state_list;
}

int StatTrace_get_loopback(const StatTrace_ptr self)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  return self->loopback;
}

int StatTrace_get_length(const StatTrace_ptr self)
{
  STAT_TRACE_CHECK_INSTANCE(self);

  return NodeList_get_length(self->sexp_state_list);
}
/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

void stat_trace_init(StatTrace_ptr self)
{
  self->generated = false;
  self->loopback = -1;

  self->sexp_state_list = NodeList_create();
  self->sexp_state_assoc = new_assoc();
}

void stat_trace_deinit(StatTrace_ptr self)
{
  NodeList_destroy(self->sexp_state_list); self->sexp_state_list = NULL;
  free_assoc(self->sexp_state_assoc); self->sexp_state_assoc = NULL;

  self->generated = false;
  self->loopback = -1;
}
/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**AutomaticEnd***************************************************************/
