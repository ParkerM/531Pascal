/*
 * FUNCTIONS.C
 * 
 * This file defines the various functions declared in
 * FUNCTIONS.H that are used to create function declarations 
 * in the PASCAL compiler.
 *
 * Authors: Michael Brunson
 *          John Mauney
 *          Myndert Papenhuyzen
 *
 *    Date: April 13, 2015
 *
 * Purpose: CSCE 531 (Compiler Construction) Project
 */

#include "functions.h"

ST_DR create_named_function(ST_ID id, PARAM_LIST params, TYPE returnType)
{
   ST_DR rec = stdr_alloc();
   rec->tag = FDECL;
   rec->u.decl->type = ty_build_func(returnType, params, FALSE);
   rec->u.decl->sc = NO_SC;
   rec->u.decl->is_ref = FALSE;
   rec->u.decl->v.global_func_name = st_get_id_str(id);
   rec->u.decl->err = FALSE;
   return rec;
}
