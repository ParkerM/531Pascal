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

#ifndef __EXPR_H
#define __EXPR_H

#include <stdlib.h>

#include "message.h"
#include "symtab.h"
#include "types.h"

/* typedef enum EXPRTAG
 *
 * This enumeration describes the various types of expressions we may hope to encounter:
 *
 *     E_ASSIGN     - An assignment statement.
 *     E_ARITH      - An arithmetic statement (involving +, -, *, /, div, mod).
 *     E_SIGN       - A unary sign statement (i.e. unary + or -).
 *     E_INTCONST   - An Integer constant.
 *     E_REALCONST  - A Real or Single constant.
 *     E_CHARCONST  - A Character constant.
 *     E_BOOLCONST  - A Boolean constant.
 *     E_PTR        - A pointer.
 *     E_COMPR      - A comparison expression (involving >, <, =, <>, <=, >=).
 *     E_UNFUNC     - A unary function (ord, chr, succ, pred).
 *     E_VAR        - A [global] variable. TODO: update for 100% level.
 *     E_FUNC       - A function call.
 *     E_ARRAY      - An array.
 *     E_CAST       - Inserted to cast types prior to operation.
 */
typedef enum {E_ASSIGN, E_ARITH, E_SIGN, E_COMPR, E_UNFUNC, 
              E_INTCONST, E_REALCONST, E_CHARCONST, E_BOOLCONST, E_PTR,
              E_VAR, E_FUNC, E_ARRAY, E_CAST} EXPRTAG; 

/* typedef enum ARITHTAG
 *
 * This enumeration differentiates the arithmetic operations.
 *
 *     AR_ADD  - Addition
 *     AR_SUM  - Subtraction
 *     AR_MULT - Multiplication
 *     AR_IDIV - Integer division ('div')
 *     AR_RDIV - Real division    ('/')
 *     AR_MOD  - Modulo division  ('mod')
 */
typedef enum {AR_ADD, AR_SUB, AR_MULT, AR_IDIV, AR_RDIV, AR_MOD} ARITHTAG;

/* typedef enum SIGNTAG
 *
 * This enumeration differentiates the unary sign operations.
 *
 *     SI_PLUS  - Unary plus
 *     SI_MINUS - Unary minus (negation)
 */
typedef enum {SI_PLUS, SI_MINUS} SIGNTAG;

/* typedef enum COMPRTAG
 *
 * This enumeration differentiates the comparison operations.
 *
 *     CM_EQUAL  - Equality relation ('=')
 *     CM_LESS   - Lesser than relation
 *     CM_GREAT  - Greated than relation
 *     CM_NEQUAL - Non-equality relation ('<>')
 *     CM_LSEQL  - Lesser than or equality relation
 *     CM_GTEQL  - Greater than or equality relation
 */
typedef enum {CM_EQUAL, CM_LESS, CM_GREAT, CM_NEQUAL, CM_LSEQL, CM_GTEQL} COMPRTAG;

/* typedef enum UNFUNCTAG
 *
 * This enumeration differentiates the unary function operations.
 *
 *     UF_ORD  - ord() [Char -> Integer]
 *     UF_CHR  - chr() [Integer -> Char]
 *     UF_SUCC - succ() [Successor function]
 *     UF_PRED - pred() [Predecessor function]
 */
typedef enum {UF_ORD, UF_CHR, UF_SUCC, UF_PRED} UNFUNCTAG;

/* typedef enum CASTTAG
 *
 * This enumeration differentiates the various type conversions that my occur.
 *
 *     CT_SGL_REAL  - Upconverts a Single to a Real.
 *     CT_REAL_SGL  - Downconverts a Real to a Single.
 *     CT_INT_REAL  - Converts an Integer to a Real.
 *     CT_LVAL_RVAL - Derefs an l-value into an r-value.
 */
typedef enum {CT_SGL_REAL, CT_REAL_SGL, CT_INT_REAL, CT_INT_SGL, CT_LDEREF} CASTTAG;

struct expr_list;

typedef struct expression
{
  EXPRTAG expr_tag;  /* What sort of expr it is. */
  TYPETAG expr_typetag; /* The expr's value typetag. */
  TYPE    expr_fulltype;
  struct  expression *left;  /* Left child */
  struct  expression *right; /* Right child and the branch for unary ops.*/
  
  union
  {
  	long integer;
  	double real;
  	char character;
    BOOLEAN bool;  // 0 = false, 1 = true;
  	
  	ARITHTAG arith_tag;
  	SIGNTAG sign_tag;
  	COMPRTAG compr_tag;
  	UNFUNCTAG unfunc_tag;
  	CASTTAG cast_tag;
  	
  	struct {
  	  ST_ID var_id;
  	  struct expr_list *arguments;
  	  BOOLEAN array_base_function;
  	} var_func_array;
  	
  } u;
} expression, *EXPR;

typedef struct expr_list
{
    expression *base;
    struct expr_list *next;
} expr_list_node, *EXPR_LIST;

/* New assignment expression */
EXPR new_expr_assign(EXPR left, EXPR right);

/* New arithmetic expression */
EXPR new_expr_arith(EXPR left, ARITHTAG t, EXPR right);

/* New casting expression */
EXPR new_expr_cast(CASTTAG t, EXPR right);

/* New signed expression */
EXPR new_expr_sign(int sign, EXPR right);

/* New int const expression */
EXPR new_expr_intconst(long i);

/* New real const expression */
EXPR new_expr_realconst(double d);

EXPR new_expr_strconst(char *str);

EXPR new_expr_boolconst(int bool);

/* New boolean expression */
EXPR new_expr_compr(EXPR left, COMPRTAG t, EXPR right);

/* New unary function expression */
EXPR new_expr_unfunc(UNFUNCTAG t, EXPR_LIST right);

/* New global variable expression */
EXPR new_expr_identifier(ST_ID id);

EXPR new_expr_var_funccall(EXPR base, EXPR_LIST arguments);

EXPR new_expr_array(EXPR base, EXPR_LIST indices);

/* Create a new list of expression nodes */
EXPR_LIST new_expr_list(EXPR item);

/* Append an expression node to an existing expression list */
EXPR_LIST append_to_expr_list(EXPR_LIST base, EXPR newItem);

#endif
