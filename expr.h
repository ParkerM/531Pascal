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

typedef struct expression
{
  EXPRTAG type;
  struct expression *left;
  struct expression *right;
  union
  {
  } u;
} expression, *EXPR;

typedef struct expr_list
{
    expression *base;
    struct expr_list *next;
} *EXPR_LIST;

/* Create a new list of expression nodes */
expr_list new_expr_list(expression item);

/* Append an expression node to an existing expression list */
expr_list append_to_expr_list(EXPR_LIST base, expression newItem);