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
   rec->u.decl.type = ty_build_func(returnType, params, FALSE);
   rec->u.decl.sc = NO_SC;
   rec->u.decl.is_ref = FALSE;
   rec->u.decl.v.global_func_name = st_get_id_str(id);
   rec->u.decl.err = FALSE;
   return rec;
}

ST_DR declare_external_function(ST_ID id, PARAM_LIST params, TYPE returnType)
{
   ST_DR rec = stdr_alloc();
   rec->tag = GDECL;
   rec->u.decl.type = ty_build_func(returnType, params, TRUE);
   rec->u.decl.sc = EXTERN_SC;
   rec->u.decl.is_ref = FALSE;
   rec->u.decl.v.global_func_name = st_get_id_str(id);
   rec->u.decl.err = FALSE;
   return rec;
}

ST_DR apply_directives(typedef_item_p funcTypeDef, DIR_LIST dirList)
{
   PARAM_LIST params = NULL;
   BOOLEAN checkArgs = FALSE;
   TYPE returnType = ty_query_func(funcTypeDef->old_type, &params, &checkArgs);
   
   DIR_LIST dir = dirList;
   ST_DR rec = NULL;
   
   while(dir != NULL && rec == NULL)
   {
      if(dir->directive == DIRECTIVE_FORWARD)
      {
         rec = declare_forward_function(funcTypeDef->new_def, params, returnType);
         st_install(funcTypeDef->new_def, rec);
      }
      else if(dir->directive == DIRECTIVE_EXTERNAL)
      {
         rec = declare_external_function(funcTypeDef->new_def, params, returnType);
         st_install(funcTypeDef->new_def, rec);
      }
      dir = dir->next;
   }
   return rec;
}

ST_DR install_function_decl(typedef_item_p funcDef)
{
   ST_ID id = funcDef->new_def;
   TYPE funcType = funcDef->old_type;
   int blockNum = 0;
   ST_DR foundRec = st_lookup(id, &blockNum);
   if(foundRec == NULL)
   {
      ST_DR rec = stdr_alloc();
      rec->tag = FDECL;
      rec->u.decl.type = funcType;
      rec->u.decl.sc = NO_SC;
      rec->u.decl.is_ref = FALSE;
      rec->u.decl.v.global_func_name = st_get_id_str(id);
      rec->u.decl.err = FALSE;
      st_install(id, rec);
      return rec;
   }
   else
   {
      if(foundRec->tag == GDECL)
      {
         int check_args1;
         int check_args2;
         PARAM_LIST params1, param1;
         PARAM_LIST params2, param2;
         
         //Must not be external
         if(foundRec->u.decl.sc == EXTERN_SC)
         {
            //duplicate function?
            error("Duplicate function declaration: %s", foundRec->u.decl.v.global_func_name);
         }    
         
         TYPETAG return1 = ty_query(ty_query_func(foundRec->u.decl.type, &params1, &check_args1));
         TYPETAG return2 = ty_query(ty_query_func(funcType, &params2, &check_args2));
         
         //test if types match here
         if(return1 != return2)
         {
            error("Multiple functions with the same name but different signatures");
         }
         
         param1 = params1;
         param2 = params2;
         while(param1 != NULL && param2 != NULL)
         {
            TYPE paramType1 = param1->type;
            TYPE paramType2 = param2->type;
            
            if(ty_query(paramType1) != ty_query(paramType2))
            {
               error("Parameter signatures do not match for forward declared function");
            }
            
            param1 = param1->next;
            param2 = param2->next;
         }         
      }
      else
      {
         //might be a duplicate function def
         error("Duplicate function declarations: %s", foundRec->u.decl.v.global_func_name);
      }
   }
   return foundRec;
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

void enter_function_block(typedef_item_p funcDef)
{
   st_enter_block();
   PARAM_LIST params = NULL;
   BOOLEAN check_args = FALSE;
   TYPE returnValue = ty_query_func(funcDef->old_type, &params, &check_args);
   
   //Setup for calculating offset value
   b_init_formal_param_offset();

   PARAM_LIST param = params;
   
   //create parameter record for each parameter and store
   while(param != NULL)
   {
   	message(st_get_id_str(param->id));
   	ty_print_typetag(ty_query(param->type));
      ST_DR rec = stdr_alloc();
      rec->tag = PDECL;
      rec->u.decl.type = param->type;
      rec->u.decl.sc = param->sc;
      rec->u.decl.is_ref = param->is_ref;
      rec->u.decl.v.offset = b_get_formal_param_offset(ty_query(param->type));      
      rec->u.decl.err = param->err;      
      st_install(param->id, rec);
      param = param->next;
   } 
}

void exit_function_block(typedef_item_p funcDef)
{
	int check_args;
   PARAM_LIST params;
   TYPE funcType = funcDef->old_type;
   TYPE returnType = ty_query_func(funcType, &params, &check_args);
   
   if(ty_query(returnType) != TYVOID)
   {
		b_prepare_return(ty_query(returnType));
	}
	
	b_func_epilogue(st_get_id_str(funcDef->new_def));   
   st_exit_block();
}

void encode_function_def(typedef_item_p funcDef)
{
   int check_args;
   PARAM_LIST params;
   TYPE funcType = funcDef->old_type;
   TYPE returnType = ty_query_func(funcType, &params, &check_args);
   PARAM_LIST param = params;
   while(param != NULL)
   {
   	message(st_get_id_str(param->id));
   	ty_print_typetag(ty_query(param->type));
   	
   	int offset = b_store_formal_param(ty_query(param->type));
   	int block;
   	if(offset != st_lookup(param->id, &block)->u.decl.v.offset)
   	{
   		bug("Offset mismatch: %s, %d", st_get_id_str(param->id), offset);
   	}
   	param = param->next;  
   }
   
   if(ty_query(returnType) != TYVOID)
   {
   	b_alloc_return_value();
   }
}

int size_of_vars(stid_list list)
{
	int size = 0;
	
	stid_list listItem = list;
	while(listItem != NULL)
	{
		ST_ID id = listItem->enrollment_papers;
		int block = 0;	
		size += get_type_size(st_lookup(id, &block)->u.decl.type);
		listItem = listItem->next;
	}
	
	
	return size;
}
