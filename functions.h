#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "message.h"
#include "symtab.h"
#include "types.h"

ST_DR create_named_function(ST_ID id, PARAM_LIST params, TYPE returnType);

#endif
