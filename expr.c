/*
 * EXPR.C
 * 
 * This header file defines the various functions declared in EXPR.H that are used to
 * create, manipulate, and traverse the expression trees created while parsing expressions
 * and statements in the PASCAL compiler.
 *
 * Authors: Michael Brunson
 *          John Mauney
 *          Myndert Papenhuyzen
 *
 *    Date: April 13, 2015
 *
 * Purpose: CSCE 531 (Compiler Construction) Project
 */
 
#include "expr.h"

int debug = 1; //set to 1 for debug messages

/* New assignment expression */
EXPR new_expr_assign(EXPR left, EXPR right) 
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_ASSIGN;
	newExpr->left = left;
	newExpr->right = right;
	if (debug == 1) msg("new_expr_assign");

	return newExpr;
}

/* New arithmetic expression */
EXPR new_expr_arith(EXPR left, ARITHTAG t, EXPR right)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->u.arith_tag = t;
	newExpr->expr_tag = E_ARITH;
	newExpr->left = left;
	newExpr->right = right;
	if (debug == 1) msg("new_expr_arith: ARITHTAG %i", newExpr->u.arith_tag);

	return newExpr;
}

/* New signed expression */
EXPR new_expr_sign(int sign, EXPR right)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_SIGN;
	SIGNTAG tag;
	if (sign == 1) {
		tag = SI_PLUS;
		newExpr->u.sign_tag = tag;		
	} else if (sign == -1) {
		tag = SI_MINUS;
		newExpr->u.sign_tag = tag;		
	} else error("incorrect sign value");
	newExpr->right = right;
	if (debug ==1) msg("new_expr_sign: SIGNTAG %i", newExpr->u.sign_tag);

	return newExpr;
}

/* New int const expression */
EXPR new_expr_intconst(long i)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_INTCONST;
	newExpr->u.integer = i;
	if (debug == 1) msg("new_expr_intconst %li", newExpr->u.integer);


	return newExpr;
}

/* New real const expression */
EXPR new_expr_realconst(double d)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_REALCONST;
	newExpr->u.real = d;
	if (debug == 1) msg("new_expr_realconst %d", newExpr->u.real);

	return newExpr;
}

/* New boolean expression */
EXPR new_expr_compr(EXPR left, COMPRTAG t, EXPR right)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_COMPR;
	newExpr->u.compr_tag = t;
	newExpr->left = left;
	newExpr->right = right;
	if (debug == 1) msg("new_expr_compr: COMPRTAG %i", newExpr->u.compr_tag);

	return newExpr;
}

/* New unary function expression */
EXPR new_expr_unfunc(UNFUNCTAG t, EXPR right)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_UNFUNC;
	newExpr->right = right;
	newExpr->u.unfunc_tag = t;
	if (debug == 1) msg("new_expr_unfunc: UNFUNCTAG %i", newExpr->u.unfunc_tag);

	return newExpr;
}

/* New global variable expression */
EXPR new_expr_var(ST_ID id, EXPR right)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_VAR;
	newExpr->right = right;
	newExpr->u.var_id = id;
	if (debug == 1) msg("new_expr_var");

	return newExpr;
}

EXPR_LIST new_expr_list(EXPR item)
{
	//allocate new EXPR_LIST
	EXPR_LIST list = (EXPR_LIST) malloc(sizeof(expr_list_node));

	//set next to null, set base node equal to item
	list->next = NULL;
	list->base = item;
	if (debug == 1) msg("new_expr_list");

	return list;
}

EXPR_LIST append_to_expr_list(EXPR_LIST list, EXPR newItem)
{
	//allocate new EXPR_LIST
	EXPR_LIST newList = (EXPR_LIST) malloc(sizeof(expr_list_node));

	//put newList at the head of list and assign the expression
	newList->next = list;
	newList->base = newItem;
	if (debug == 1) msg("append_to_expr_list");

	return newList;
}