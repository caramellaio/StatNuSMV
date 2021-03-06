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
  \brief Statistical model checking sexpr problem generation functions

  Statistical model checking sexpr problem generation functions
*/

#include "nusmv/core/stat/statInt.h"
#include "nusmv/core/wff/ExprMgr.h"
#include "nusmv/core/node/NodeMgr.h"
#include "nusmv/core/parser/symbols.h"
#include "nusmv/core/compile/compile.h"

#include "nusmv/core/utils/error.h" /* for error_unreachable_code_msg */

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

SymbLayer_ptr StatSexpProblem_prepare_layer(const NuSMVEnv_ptr env,
                                            SymbTable_ptr symb_table,
                                            const Expr_ptr counter_var,
                                            const int k)
{
  const NodeMgr_ptr nodemgr = NODE_MGR(NuSMVEnv_get_value(env, ENV_NODE_MGR));
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));
  const BoolEnc_ptr bool_enc = BOOL_ENC(NuSMVEnv_get_value(env, ENV_BOOL_ENCODER));
  //const BeEnc_ptr be_enc = BE_ENC(NuSMVEnv_get_value(env, ENV_BE_ENCODER));
  const BddEnc_ptr bdd_enc = BDD_ENC(NuSMVEnv_get_value(env, ENV_BDD_ENCODER));

  SymbLayer_ptr retval;
  SymbType_ptr counter_type;

  nusmv_assert(1 < k);

  retval = SymbTable_create_layer(symb_table, NULL, SYMB_LAYER_POS_BOTTOM);

  /* symbol type: 1 .. k */
  counter_type =
    Compile_InstantiateType(symb_table, retval, counter_var,
                            new_node(nodemgr, TWODOTS,
                                     ExprMgr_number(exprs, 1),
                                     ExprMgr_number(exprs, k)),
                            NULL, false);

  if (! Compile_DeclareVariable(symb_table, retval, counter_var, counter_type,
                                NULL, State_Variables_Instantiation_Mode)) {
    error_unreachable_code_msg("This should not happen!!! \n");
  }


  BaseEnc_commit_layer(BASE_ENC(bool_enc), SymbLayer_get_name(retval));
  //BaseEnc_commit_layer(BASE_ENC(be_enc), SymbLayer_get_name(retval));
  BaseEnc_commit_layer(BASE_ENC(bdd_enc), SymbLayer_get_name(retval));

  return retval;

}

void StatSexpProblem_destroy_layer(const NuSMVEnv_ptr env,
                                   SymbTable_ptr symb_table,
                                   SymbLayer_ptr layer)
{
  const BoolEnc_ptr bool_enc = BOOL_ENC(NuSMVEnv_get_value(env, ENV_BOOL_ENCODER));
  //const BeEnc_ptr be_enc = BE_ENC(NuSMVEnv_get_value(env, ENV_BE_ENCODER));
  const BddEnc_ptr bdd_enc = BDD_ENC(NuSMVEnv_get_value(env, ENV_BDD_ENCODER));

  const char* layer_name = SymbLayer_get_name(layer);

  nusmv_assert(SymbTable_has_layer(symb_table, layer_name));

  BaseEnc_remove_layer(BASE_ENC(bdd_enc), layer_name);
  //BaseEnc_remove_layer(BASE_ENC(be_enc), layer_name);
  BaseEnc_remove_layer(BASE_ENC(bool_enc), layer_name);

  SymbTable_remove_layer(symb_table, layer);
}

Prop_ptr StatSexpProblem_gen_problem(const NuSMVEnv_ptr env,
                                     const StatTrace_ptr stat_trace,
                                     const Expr_ptr counter_var,
                                     const Prop_ptr prop)
{
  Expr_ptr counter_next, counter_init, state_counter_mapping;
  Expr_ptr state_trace_problem, final_formula;

  const NodeMgr_ptr nodemgr = NODE_MGR(NuSMVEnv_get_value(env, ENV_NODE_MGR));
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));

  const int k = StatTrace_get_length(stat_trace);
  const int l = StatTrace_get_loopback(stat_trace);

  /* Only LTL formula are supported */
  nusmv_assert(Prop_Ltl == Prop_get_type(prop));

  /* We consider only infinite trace */
  nusmv_assert(StatTrace_is_generated(stat_trace));

  /* counter init: counter_var = 1 */
  counter_init = find_node(nodemgr, EQUAL, counter_var, ExprMgr_number(exprs, 1));

  /* counter_next: ((counter_var = k) -> next(counter_var) = l) &
                   ((counter_var != k) -> next(counter_var) = counter_var + 1) */
  {
    Expr_ptr next_counter_var = find_node(nodemgr, NEXT, counter_var, Nil);
    Expr_ptr counter_is_k =
      find_node(nodemgr, EQUAL, counter_var, ExprMgr_number(exprs, k));
    Expr_ptr case_k = ExprMgr_implies(exprs, counter_is_k,
                                      find_node(nodemgr, EQUAL, next_counter_var,
                                                ExprMgr_number(exprs, l)));

    Expr_ptr case_not_k =
      ExprMgr_implies(exprs, ExprMgr_not(exprs, counter_is_k),
                      find_node(nodemgr, EQUAL, next_counter_var,
                                    ExprMgr_plus(exprs, counter_var,
                                                 ExprMgr_number(exprs, 1))));

    counter_next = ExprMgr_and(exprs, case_k, case_not_k);
  }

  /* Add " G state_i iff counter_i" mapping */
  {
    ListIter_ptr iter;
    int counter_val = 1;

    NodeList_ptr state_sexp_list = StatTrace_get_sexp_states(stat_trace);

    state_counter_mapping = ExprMgr_true(exprs);

    NODE_LIST_FOREACH(state_sexp_list, iter) {
      /* big_and(v in vars, v.at_state(i) */
      Expr_ptr sexp_state = NodeList_get_elem_at(state_sexp_list, iter);
      /* counter_var = i */
      Expr_ptr counter_var_is_i =
        find_node(nodemgr, EQUAL, counter_var, ExprMgr_number(exprs, counter_val));
      /* counter_var_is_i implies sexp_state */
      Expr_ptr counter_i_expr = ExprMgr_implies(exprs, counter_var_is_i, sexp_state);

      /* apply conjunction */
      state_counter_mapping =
        ExprMgr_and(exprs, counter_i_expr, state_counter_mapping);
      counter_val++;
    }

  }

  /* trace problem: counter_init & G (counter_next & state_counter_mapping) */
  state_trace_problem = ExprMgr_and(exprs, counter_init,
                                    find_node(nodemgr, OP_GLOBAL,
                                              ExprMgr_and(exprs, counter_next,
                                                          state_counter_mapping),
                                              Nil));

  /* final_formula: trace_problem implies prop_formula */
  final_formula =
    ExprMgr_implies(exprs, state_trace_problem, Prop_get_expr_core(prop));

  return Prop_create_partial(env, final_formula, Prop_Ltl);
}

Prop_ptr StatSexpProblem_gen_single_state_problem(const NuSMVEnv_ptr env,
                                                  const StatTrace_ptr execution,
                                                  const Prop_ptr prop)
{
  NodeList_ptr sexp_state_list;
  ListIter_ptr iter;
  Expr_ptr state_sexp, final_formula;

  const NodeMgr_ptr nodemgr = NODE_MGR(NuSMVEnv_get_value(env, ENV_NODE_MGR));
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));

  nusmv_assert(StatTrace_get_length(execution) == 1);
  nusmv_assert(Prop_get_type(prop) == Prop_Ltl);

  sexp_state_list = StatTrace_get_sexp_states(execution);
  iter = NodeList_get_first_iter(sexp_state_list);
  state_sexp = NodeList_get_elem_at(sexp_state_list, iter);

  /* G (state_sexp) -> property */
  final_formula =
    ExprMgr_implies(exprs,
                    find_node(nodemgr, OP_GLOBAL, state_sexp, Nil),
                    Prop_get_expr_core(prop));

  return Prop_create_partial(env, final_formula, Prop_Ltl);
}

Prop_ptr StatSexpProblem_gen_bmc_problem(const NuSMVEnv_ptr env,
                                         const StatTrace_ptr execution,
                                         const Prop_ptr prop)
{
  const NodeMgr_ptr nodemgr = NODE_MGR(NuSMVEnv_get_value(env, ENV_NODE_MGR));
  const ExprMgr_ptr exprs = EXPR_MGR(NuSMVEnv_get_value(env, ENV_EXPR_MANAGER));


  Expr_ptr final_formula = ExprMgr_true(exprs);

  nusmv_assert(Prop_get_type(prop) == Prop_Ltl);

  {
    ListIter_ptr iter;
    int counter_val = 1;

    NodeList_ptr state_sexp_list = StatTrace_get_sexp_states(execution);

    /* we start computing from the last state to the first one */
    NodeList_reverse(state_sexp_list);

    NODE_LIST_FOREACH(state_sexp_list, iter) {
      Expr_ptr sexp_state = NodeList_get_elem_at(state_sexp_list, iter);

      if (ExprMgr_is_true(exprs, final_formula)) {
        final_formula = sexp_state;
      }
      else {
        /* state_ith & X (final_formula) */
        final_formula =
          ExprMgr_and(exprs, sexp_state,
                      find_node(nodemgr, OP_NEXT, final_formula, Nil));
      }
    }

    /* restore the state_sexp_list */
    NodeList_reverse(state_sexp_list);
  }

  /* states implies property */
  final_formula =
    ExprMgr_implies(exprs, final_formula, Prop_get_expr_core(prop));
  return Prop_create_partial(env, final_formula, Prop_Ltl);
}
