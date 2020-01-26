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
  \brief Model Checker statistical verification command

  This file contains commands to be used for the statistical verification feature.

*/

#include "nusmv/shell/cmd/cmd.h"
#include "nusmv/shell/stat/statCmd.h"

#include "nusmv/core/utils/OStream.h"
#include "nusmv/core/utils/StreamMgr.h"
#include "nusmv/core/utils/Logger.h"
#include "nusmv/core/utils/ErrorMgr.h"
#include "nusmv/core/utils/error.h" /* for CATCH(errmgr) */

#include "nusmv/core/prop/Prop.h"
#include "nusmv/core/prop/propPkg.h"
#include "nusmv/core/prop/PropDb.h"

#include "nusmv/core/compile/compile.h"

#include "nusmv/core/stat/stat.h"
/* Prototypes of command functions */
int Stat_CommandCheckLtlspec(NuSMVEnv_ptr env, int argc, char** argv);
int Stat_CommandCheckLtlspecBmc(NuSMVEnv_ptr env, int argc, char** argv);

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
extern cmp_struct_ptr cmps;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define STAT_CMD_DEFAULT_EPSILON 0.9
#define STAT_CMD_DEFAULT_DELTA 0.9
/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static int UsageStatCheckLtlspec(const NuSMVEnv_ptr env);

static inline void print_result(const StreamMgr_ptr streams,
                                const OptsHandler_ptr opts,
                                const Prop_ptr prop,
                                const double delta,
                                const double epsilon,
                                const double result);
/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

void Stat_Cmd_init(NuSMVEnv_ptr env)
{
  Cmd_CommandAdd(env, "check_ltlspec_stat", Stat_CommandCheckLtlspec, 0, true);
  Cmd_CommandAdd(env, "check_ltlspec_stat_bmc", Stat_CommandCheckLtlspecBmc, 0, true);
}

void Stat_Cmd_quit(NuSMVEnv_ptr env)
{
  boolean status = true;

  status = status && Cmd_CommandRemove(env, "check_ltlspec_stat");
  status = status && Cmd_CommandRemove(env, "check_ltlspec_stat_bmc");

  nusmv_assert(status);
}

int Stat_CommandCheckLtlspecBmc(NuSMVEnv_ptr env, int argc, char** argv)
{
  const StreamMgr_ptr streams =
    STREAM_MGR(NuSMVEnv_get_value(env, ENV_STREAM_MANAGER));
  const ErrorMgr_ptr errmgr =
    ERROR_MGR(NuSMVEnv_get_value(env, ENV_ERROR_MANAGER));
  int c;
  int prop_no = -1;
  double delta;
  double epsilon;
  char* formula = NIL(char);
  char* formula_name = NIL(char);
  int status = 0;
  int useMore = 0;
  char* dbgFileName = NIL(char);
  FILE* outstream = StreamMgr_get_output_stream(streams);
  FILE* old_outstream = outstream;
  SymbTable_ptr st = SYMB_TABLE(NuSMVEnv_get_value(env, ENV_SYMB_TABLE));
  PropDb_ptr prop_db = PROP_DB(NuSMVEnv_get_value(env, ENV_PROP_DB));
  OptsHandler_ptr opts = OPTS_HANDLER(NuSMVEnv_get_value(env, ENV_OPTS_HANDLER));
  epsilon = STAT_CMD_DEFAULT_EPSILON;
  delta = STAT_CMD_DEFAULT_DELTA;

  util_getopt_reset();
  while ((c = util_getopt(argc,argv,"hmo:n:p:P:")) != EOF) {

    switch (c) {
    case 'h': return UsageStatCheckLtlspec(env);

    case 'd':
      delta = atof(util_optarg);

      if (0 >= delta || 1 <= delta) {
        StreamMgr_print_error(streams, "Delta value must be > 0 and < 1");
        return 1;
      }
      break;
    case 'e':
      epsilon = atof(util_optarg);
      if (0 >= epsilon || 1 <= epsilon) {
        StreamMgr_print_error(streams, "Epsilon value must be > 0 and < 1");
        return 1;
      }
      break;
    case 'n':
      if (formula != NIL(char)) return UsageStatCheckLtlspec(env);
      if (prop_no != -1) return UsageStatCheckLtlspec(env);
      if (formula_name != NIL(char)) return UsageStatCheckLtlspec(env);

      prop_no = PropDb_get_prop_index_from_string(prop_db, util_optarg);
      if (-1 == prop_no)
        return 1;

      break;

    case 'P':
        if (formula != NIL(char)) return UsageStatCheckLtlspec(env);
        if (prop_no != -1) return UsageStatCheckLtlspec(env);
        if (formula_name != NIL(char)) return UsageStatCheckLtlspec(env);

        formula_name = util_strsav(util_optarg);
        prop_no = PropDb_prop_parse_name(prop_db, formula_name);

        if (prop_no == -1) {
          StreamMgr_print_error(streams,  "No property named \"%s\"\n", formula_name);
          FREE(formula_name);
          return 1;
        }
        FREE(formula_name);
        break;

    case 'p':
      if (prop_no != -1) return UsageStatCheckLtlspec(env);
      if (formula != NIL(char)) return UsageStatCheckLtlspec(env);
      if (formula_name != NIL(char)) return UsageStatCheckLtlspec(env);

      formula = util_strsav(util_optarg);
      break;

    case 'o':
      if (useMore == 1) return UsageStatCheckLtlspec(env);
      dbgFileName = util_strsav(util_optarg);
      StreamMgr_print_output(streams,  "Output to file: %s\n", dbgFileName);
      break;

    case 'm':
      if (dbgFileName != NIL(char)) return UsageStatCheckLtlspec(env);
      useMore = 1;
      break;

    default:  return UsageStatCheckLtlspec(env);
    }
  }
  if (argc != util_optind) return UsageStatCheckLtlspec(env);

  if (cmp_struct_get_read_model(cmps) == 0) {
    StreamMgr_print_error(streams,
            "A model must be read before. Use the \"read_model\" command.\n");
    return 1;
  }

  if (cmp_struct_get_encode_variables(cmps) == 0) {
    StreamMgr_print_error(streams,
            "The variables must be built before. Use the \"encode_variables\" command.\n");
    return 1;
  }

  if ( (!cmp_struct_get_build_model(cmps))
       && (opt_cone_of_influence(opts) == false) ) {
    StreamMgr_print_error(streams,  "The current partition method %s has not yet be computed.\n",
            TransType_to_string(get_partition_method(opts)));
    StreamMgr_print_error(streams,  "Use \t \"build_model -f -m %s\"\nto build the transition relation.\n",
            TransType_to_string(get_partition_method(opts)));
    return 1;
  }

  if (useMore || (char*)NULL != dbgFileName) {
    if (OUTCOME_SUCCESS !=
        Cmd_Misc_open_pipe_or_file(env, dbgFileName, &outstream)) {
      status = 1; goto stat_check_ltlspec_bmc_exit;
    }
  }


  Stat_Pkg_init(env);

  if (formula != NIL(char)) {
    prop_no = PropDb_prop_parse_and_add(prop_db, st, formula, Prop_Ltl, Nil);

    if (prop_no == -1) { status = 1; goto stat_check_ltlspec_bmc_exit; }

    CATCH(errmgr) {
      double result;
      Prop_ptr p = PropDb_get_prop_at_index(prop_db, prop_no);
      status = Stat_check_ltlspec_bmc(env, p, epsilon, delta, &result);

      print_result(streams, opts, p, delta, epsilon, result);
    }
    FAIL(errmgr) {
      status = 1;
    }
  }
  else if (prop_no != -1) {
    if (Prop_check_type(PropDb_get_prop_at_index(
                  prop_db, prop_no), Prop_Ltl) != 0) {
      status = 1;
    }
    else {
      CATCH(errmgr) {
        double result;
        Prop_ptr p = PropDb_get_prop_at_index(prop_db, prop_no);

        status = Stat_check_ltlspec_bmc(env, p, epsilon, delta, &result);

        print_result(streams, opts, p, delta, epsilon, result);
      }
      FAIL(errmgr) {
        status = 1;
      }
    }
  }
  else {
    CATCH(errmgr) {
      int i;

      for (i = 0; i < PropDb_get_size(prop_db); i++) {
        double result;
        Prop_ptr p = PropDb_get_prop_at_index(prop_db, i);

        status = 0;
        if (Prop_get_type(p) == Prop_Ltl) {
          status = Stat_check_ltlspec_bmc(env, p, delta, epsilon, &result);

          if (1 == status) { goto stat_check_ltlspec_bmc_exit; }

          print_result(streams, opts, p, delta, epsilon, result);
        }
      }
    }
    FAIL(errmgr) {
      status = 1;
    }
  }

  Stat_Pkg_quit(env);

stat_check_ltlspec_bmc_exit:
  if (useMore) {
    FILE* reset_stream;

    CmdClosePipe(outstream);
    reset_stream = StreamMgr_reset_output_stream(streams);
    StreamMgr_set_output_stream(streams, old_outstream);

    nusmv_assert(reset_stream == outstream);

    outstream = (FILE*)NULL;
  }

  if ((char*)NULL != dbgFileName) {
    /* this closes the file stream as well  */
    StreamMgr_set_output_stream(streams, old_outstream);

    outstream = (FILE*)NULL;
  }

  return status;
}

int Stat_CommandCheckLtlspec(NuSMVEnv_ptr env, int argc, char** argv)
{
  const StreamMgr_ptr streams =
    STREAM_MGR(NuSMVEnv_get_value(env, ENV_STREAM_MANAGER));
  const ErrorMgr_ptr errmgr =
    ERROR_MGR(NuSMVEnv_get_value(env, ENV_ERROR_MANAGER));
  int c;
  int prop_no = -1;
  double delta;
  double epsilon;
  char* formula = NIL(char);
  char* formula_name = NIL(char);
  int status = 0;
  int useMore = 0;
  char* dbgFileName = NIL(char);
  FILE* outstream = StreamMgr_get_output_stream(streams);
  FILE* old_outstream = outstream;
  SymbTable_ptr st = SYMB_TABLE(NuSMVEnv_get_value(env, ENV_SYMB_TABLE));
  PropDb_ptr prop_db = PROP_DB(NuSMVEnv_get_value(env, ENV_PROP_DB));
  OptsHandler_ptr opts = OPTS_HANDLER(NuSMVEnv_get_value(env, ENV_OPTS_HANDLER));

  epsilon = STAT_CMD_DEFAULT_EPSILON;
  delta = STAT_CMD_DEFAULT_DELTA;

  util_getopt_reset();
  while ((c = util_getopt(argc,argv,"hmo:n:p:P:")) != EOF) {

    switch (c) {
    case 'h': return UsageStatCheckLtlspec(env);

    case 'd':
      delta = atof(util_optarg);

      if (0 >= delta || 1 <= delta) {
        StreamMgr_print_error(streams, "Delta value must be > 0 and < 1");
        return 1;
      }
      break;
    case 'e':
      epsilon = atof(util_optarg);
      if (0 >= epsilon || 1 <= epsilon) {
        StreamMgr_print_error(streams, "Epsilon value must be > 0 and < 1");
        return 1;
      }
      break;
    case 'n':
      if (formula != NIL(char)) return UsageStatCheckLtlspec(env);
      if (prop_no != -1) return UsageStatCheckLtlspec(env);
      if (formula_name != NIL(char)) return UsageStatCheckLtlspec(env);

      prop_no = PropDb_get_prop_index_from_string(prop_db, util_optarg);
      if (-1 == prop_no)
        return 1;

      break;

    case 'P':
        if (formula != NIL(char)) return UsageStatCheckLtlspec(env);
        if (prop_no != -1) return UsageStatCheckLtlspec(env);
        if (formula_name != NIL(char)) return UsageStatCheckLtlspec(env);

        formula_name = util_strsav(util_optarg);
        prop_no = PropDb_prop_parse_name(prop_db, formula_name);

        if (prop_no == -1) {
          StreamMgr_print_error(streams,  "No property named \"%s\"\n", formula_name);
          FREE(formula_name);
          return 1;
        }
        FREE(formula_name);
        break;

    case 'p':
      if (prop_no != -1) return UsageStatCheckLtlspec(env);
      if (formula != NIL(char)) return UsageStatCheckLtlspec(env);
      if (formula_name != NIL(char)) return UsageStatCheckLtlspec(env);

      formula = util_strsav(util_optarg);
      break;

    case 'o':
      if (useMore == 1) return UsageStatCheckLtlspec(env);
      dbgFileName = util_strsav(util_optarg);
      StreamMgr_print_output(streams,  "Output to file: %s\n", dbgFileName);
      break;

    case 'm':
      if (dbgFileName != NIL(char)) return UsageStatCheckLtlspec(env);
      useMore = 1;
      break;

    default:  return UsageStatCheckLtlspec(env);
    }
  }
  if (argc != util_optind) return UsageStatCheckLtlspec(env);

  if (cmp_struct_get_read_model(cmps) == 0) {
    StreamMgr_print_error(streams,
            "A model must be read before. Use the \"read_model\" command.\n");
    return 1;
  }

  if (cmp_struct_get_encode_variables(cmps) == 0) {
    StreamMgr_print_error(streams,
            "The variables must be built before. Use the \"encode_variables\" command.\n");
    return 1;
  }

  if ( (!cmp_struct_get_build_model(cmps))
       && (opt_cone_of_influence(opts) == false) ) {
    StreamMgr_print_error(streams,  "The current partition method %s has not yet be computed.\n",
            TransType_to_string(get_partition_method(opts)));
    StreamMgr_print_error(streams,  "Use \t \"build_model -f -m %s\"\nto build the transition relation.\n",
            TransType_to_string(get_partition_method(opts)));
    return 1;
  }

  if (useMore || (char*)NULL != dbgFileName) {
    if (OUTCOME_SUCCESS !=
        Cmd_Misc_open_pipe_or_file(env, dbgFileName, &outstream)) {
      status = 1; goto stat_check_ltlspec_exit;
    }
  }

  Stat_Pkg_init(env);

  if (formula != NIL(char)) {
    prop_no = PropDb_prop_parse_and_add(prop_db, st,
                                        formula, Prop_Ltl, Nil);

    if (prop_no == -1) { status = 1; goto stat_check_ltlspec_exit; }

    CATCH(errmgr) {
      double result;
      Prop_ptr p = PropDb_get_prop_at_index(prop_db, prop_no);
      status = Stat_check_ltlspec(env, p, epsilon, delta, &result);

      print_result(streams, opts, p, delta, epsilon, result);
    }
    FAIL(errmgr) {
      status = 1;
    }
  }
  else if (prop_no != -1) {
    if (Prop_check_type(PropDb_get_prop_at_index(prop_db, prop_no),
                                                 Prop_Ltl) != 0) {
      status = 1;
    }
    else {
      CATCH(errmgr) {
        double result;
        Prop_ptr p = PropDb_get_prop_at_index(prop_db, prop_no);

        status = Stat_check_ltlspec(env, p, epsilon, delta, &result);

        print_result(streams, opts, p, delta, epsilon, result);
      }
      FAIL(errmgr) {
        status = 1;
      }
    }
  }
  else {
    CATCH(errmgr) {
      int i;

      for (i = 0; i < PropDb_get_size(prop_db); i++) {
        double result;
        Prop_ptr p = PropDb_get_prop_at_index(prop_db, i);

        status = 0;
        if (Prop_get_type(p) == Prop_Ltl) {
          status = Stat_check_ltlspec(env, p, delta, epsilon, &result);

          if (1 == status) { goto stat_check_ltlspec_exit; }

          print_result(streams, opts, p, delta, epsilon, result);
        }
      }
    }
    FAIL(errmgr) {
      status = 1;
    }
  }

stat_check_ltlspec_exit:
  if (useMore) {
    FILE* reset_stream;

    CmdClosePipe(outstream);
    reset_stream = StreamMgr_reset_output_stream(streams);
    StreamMgr_set_output_stream(streams, old_outstream);

    nusmv_assert(reset_stream == outstream);

    outstream = (FILE*)NULL;
  }

  if ((char*)NULL != dbgFileName) {
    /* this closes the file stream as well  */
    StreamMgr_set_output_stream(streams, old_outstream);

    outstream = (FILE*)NULL;
  }

  Stat_Pkg_quit(env);

  return status;
}


static int UsageStatCheckLtlspec(const NuSMVEnv_ptr env)
{
  return 1;
}

static inline void print_result(const StreamMgr_ptr streams,
                                const OptsHandler_ptr opts,
                                const Prop_ptr prop,
                                const double delta,
                                const double epsilon,
                                const double result)
{

  StreamMgr_print_output(streams, "Statistical verification of : ");
  Prop_print(prop,StreamMgr_get_output_ostream(streams),
             get_prop_print_method(opts));
  StreamMgr_print_output(streams, "\nwith parameters delta = %f and epsilon = %f is %f\n",
                                  delta, epsilon, result);
}
