#ifndef ENCODE_H
#define ENCODE_H

#include "backend-x86.h"
#include "defs.h"
#include "types.h"
#include "symtab.h"
#include "message.h"

void encode(ST_DR rec);
void encode_decl_from_type(TYPE type);
int get_type_size(TYPE type);
int get_type_alignment(TYPE type);

#endif
