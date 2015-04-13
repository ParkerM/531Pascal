/*
 * EXPR.H
 * 
 * This header file declares the various structures and functions used to create, manipulate,
 * and traverse the expression trees created while parsing expressions and statements in
 * the PASCAL compiler.
 *
 * Authors: Michael Brunson
 *          John Mauney
 *          Myndert Papenhuyzen
 *
 *    Date: April 13, 2015
 *
 * Purpose: CSCE 531 (Compiler Construction) Project
 */

#include "encode.h"
#include "message.h"
#include "symtab.h"
#include "tree.h"
#include "types.h"

typedef enum EXPRTAG {E_ASSIGN, E_ARITH, E_SIGN, E_INTCONST, E_REALCONST, E_COMPR, E_UNFUNC, E_VAR}; 

typedef struct Expression
{
  EXPRTAG type;
  union
  {
  } u;
} Expression, *EXPR;
