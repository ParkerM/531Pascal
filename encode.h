#ifndef ENCODE_H
#define ENCODE_H

#include "backend-x86.h"
#include "defs.h"
#include "expr.h"
#include "types.h"
#include "symtab.h"
#include "message.h"
#include <string.h>

void encode(ST_ID id);
void encode_decl_from_type(TYPE type);
void encode_expression(EXPR expr);
int get_type_size(TYPE type);
int get_type_alignment(TYPE type);

void start_main();
void end_main();

//store last loop exit label for break statements
void store_label(char* label);

//get last loop exit label for break statements
char *get_last_label();

#endif
