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

ST_DR declare_forward_function(ST_ID id, PARAM_LIST params, TYPE returnType)
{
   ST_DR rec = stdr_alloc();
   rec->tag = GDECL;
   rec->u.decl->type = ty_build_func(returnType, params, FALSE);
   rec->u.decl->sc = NO_SC;
   rec->u.decl->is_ref = FALSE;
   rec->u.decl->v.global_func_name = st_get_id_str(id);
   rec->u.decl->err = FALSE;
   return rec;
}

ST_DR declare_external_function(ST_ID id, PARAM_LIST params, TYPE returnType)
{
   ST_DR rec = stdr_alloc();
   rec->tag = GDECL;
   rec->u.decl->type = ty_build_func(returnType, params, TRUE);
   rec->u.decl->sc = EXTERN_SC;
   rec->u.decl->is_ref = FALSE;
   rec->u.decl->v.global_func_name = st_get_id_str(id);
   rec->u.decl->err = FALSE;
   return rec;
}

void apply_directives(typedef_item_p funcTypeDef, DIR_LIST dirList)
{
   PARAM_LIST params = NULL;
   BOOLEAN checkArgs = FALSE;
   ty_query_func(funcTypeDef->old_type, &params, &checkArgs);
   
   DIR_LIST dir = dirList;
   ST_DR rec = NULL;
   
   while(dir && rec == NULL)
   {
      if(dir->directive == DIRECTIVE_FORWARD)
      {
         rec = declare_forward_function(funcTypeDef->new_def, params,funcTypeDef->old_type);
         st_install(funcTypeDef->new_def, rec);
      }
      else if(dir->directive == DIRECTIVE_EXTERNAL)
      {
         rec = declare_external_function(funcTypeDef->new_def, params,funcTypeDef->old_type);
         st_install(funcTypeDef->new_def, rec);
      }
      dir = dir->next;
   }
}

ST_DR install_function_decl(ST_ID id)
{
   
}

DIR_LIST create_dir_list(DIRECTIVETYPE type)
{
   DIR_LIST list = (DIR_LIST)malloc(sizeof(DIRECTIVE));
   list->directive = type;
   list->next = NULL;
   return list;
}

DIR_LIST append_to_dir_list(DIR_LIST list, DIRECTIVETYPE type)
{
   DIR_LIST newList = create_dir_list(type);
   newList->next = list;
   return newList;
}
