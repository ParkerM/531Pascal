#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "message.h"
#include "symtab.h"
#include "types.h"
#include "tree.h"

typedef enum {
  DIRECTIVE_FORWARD, DIRECTIVE_EXTERNAL
} DIRECTIVETYPE;

typedef struct dir {
  DIRECTIVETYPE directive;
  struct dir *next;
} DIRECTIVE, *DIR_LIST;

ST_DR declare_forward_function(ST_ID id, PARAM_LIST params, TYPE returnType);
ST_DR declare_external_function(ST_ID id, PARAM_LIST params, TYPE returnType);
ST_DR apply_directives(typedef_item_p funcTypeDef, DIR_LIST directives);
ST_DR install_function_decl(typedef_item_p id);

void enter_function_block(typedef_item_p params);
void exit_function_block(typedef_item_p funcTypeDef);
void encode_function_def(typedef_item_p funcDef);
int size_of_vars(stid_list list);
DIR_LIST create_dir_list(DIRECTIVETYPE type);
DIR_LIST append_to_dir_list(DIR_LIST list, DIRECTIVETYPE type);

#endif
