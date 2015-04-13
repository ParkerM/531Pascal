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

/* New assignment expression */
EXPR new_expr_assign(EXPR left, EXPR right) {
	
}

EXPR_LIST new_expr_list(expression item)
{
	//allocate new EXPR_LIST
	EXPR_LIST list = (EXPR_LIST) malloc(sizeof(expr_list_node));

	//set next to null, set base node equal to item
	list->next = NULL;
	list->base = item;

	return list;
}

EXPR_LIST append_to_expr_list(EXPR_LIST list, expression newItem)
{
	//allocate new EXPR_LIST
	EXPR_LIST newList = (EXPR_LIST) malloc(sizeof(expr_list_node));

	//put newList at the head of list and assign the expression
	newList->next = list;
	newList->base = newItem;

	return newList;
}