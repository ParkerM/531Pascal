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

#define COMPLETELY_INCOMPATIBLE -1
#define COMPLETELY_COMPATIBLE    0
#define CONVERSION_REQUIRED      1

/* Analyzes the two expressions to determine if a type conversion for either is required.
 * 
 * left, right -- the two expressions to compare.
 * predence    -- flag determining which side's type should be the "base." Used for assignment or parameters.
 *                -1 => left; 1 => right; 0 => none. Under no precdence, defaults to Double.
 * required    -- output TYPETAG set to the earliest common "parent" type.
 * 
 * Returns:
 *   COMPLETELY_INCOMPATIBLE -- if types are not compatible and no conversion can fix the problem.
 *   COMPLETELY_COMPATIBLE   -- if types are compatible and no conversion required.
 *   CONVERSION_REQUIRED     -- if types are not compatible, but a conversion can be done.
 */
int require_type_conversion(EXPR left, EXPR right, int precedence, TYPETAG *required);

int debug = 1; //set to 1 for debug messages

/* New assignment expression */
EXPR new_expr_assign(EXPR left, EXPR right) 
{
    TYPETAG required;
    int compatible = require_type_conversion(left, right, -1, &required);
    
    EXPR modifiedRight = right;
    
    if (right->expr_tag == E_VAR)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
    if (compatible == COMPLETELY_INCOMPATIBLE)
    {
        error("Illegal conversion between types.");
        ty_print_typetag(left->expr_type);
        ty_print_typetag(modifiedRight->expr_type);
    }
    else if (compatible == CONVERSION_REQUIRED)
    {
        if (modifiedRight->expr_type == TYFLOAT && required == TYDOUBLE)
        {
            modifiedRight = new_expr_cast(CT_SGL_REAL, modifiedRight);
        }
        else if (modifiedRight->expr_type == TYDOUBLE && required == TYFLOAT)
        {
            modifiedRight = new_expr_cast(CT_REAL_SGL, modifiedRight);
        }
        else if (modifiedRight->expr_type == TYSIGNEDLONGINT && required == TYDOUBLE)
        {
            modifiedRight = new_expr_cast(CT_INT_REAL, modifiedRight);
        }
    }
    
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_ASSIGN;
	newExpr->left = left;
	newExpr->right = modifiedRight;
	if (debug == 1) msg("new_expr_assign");

	return newExpr;
}

/* New arithmetic expression */
EXPR new_expr_arith(EXPR left, ARITHTAG t, EXPR right)
{
    TYPETAG required;
    int compatible = require_type_conversion(left, right, 0, &required);
    
    EXPR modifiedLeft = left;
    EXPR modifiedRight = right;
    
    if (left->expr_tag == E_VAR)
    {
        modifiedLeft = new_expr_cast(CT_LDEREF, left);
    }
    
    if (right->expr_tag == E_VAR)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
    if (compatible == COMPLETELY_INCOMPATIBLE)
    {
        error("Illegal conversion between types.");
        ty_print_typetag(modifiedLeft->expr_type);
        ty_print_typetag(modifiedRight->expr_type);
    }
    else if (compatible == CONVERSION_REQUIRED)
    {
        if (modifiedRight->expr_tag == TYFLOAT) modifiedRight = new_expr_cast(CT_SGL_REAL, modifiedRight);
        else if (modifiedRight->expr_tag == TYSIGNEDLONGINT) modifiedRight = new_expr_cast(CT_INT_REAL, modifiedRight);
        
        if (modifiedLeft->expr_tag == TYFLOAT) modifiedRight = new_expr_cast(CT_SGL_REAL, modifiedLeft);
        else if (modifiedLeft->expr_tag == TYSIGNEDLONGINT) modifiedRight = new_expr_cast(CT_INT_REAL, modifiedLeft);
    }
    
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
    EXPR modifiedRight = right;
    
    if (right->expr_tag == E_VAR)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
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
    newExpr->expr_type = modifiedRight->expr_type;
	newExpr->right = modifiedRight;
	if (debug ==1) msg("new_expr_sign: SIGNTAG %i", newExpr->u.sign_tag);

	return newExpr;
}

/* New int const expression */
EXPR new_expr_intconst(long i)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_INTCONST;
    newExpr->expr_type = TYSIGNEDLONGINT;
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
    newExpr->expr_type = TYDOUBLE;
	newExpr->u.real = d;
	if (debug == 1) msg("new_expr_realconst %f", newExpr->u.real);

	return newExpr;
}

/* new character constant expression */
EXPR new_expr_strconst(char *str)
{
    EXPR newExpr = (EXPR) malloc(sizeof(expression));
    
    newExpr->expr_tag = E_CHARCONST;
    newExpr->expr_type = TYUNSIGNEDCHAR;
    newExpr->u.character = str[0];
    if (debug) msg("new_expr_strconst %s", newExpr->u.character);
    
    return newExpr;
}

/* New boolean constant (i.e. TRUE or FALSE). */
EXPR new_expr_boolconst(int bool)
{
    EXPR newExpr = (EXPR) malloc(sizeof(expression));
    
    newExpr->expr_tag = E_BOOLCONST;
    newExpr->expr_type = TYSIGNEDCHAR;
    newExpr->u.bool = bool;
    if (debug == 1) msg("new_expr_boolconst %s", (bool == 0) ? "false" : "true");
    
    return newExpr;
}

/* New boolean expression */
EXPR new_expr_compr(EXPR left, COMPRTAG t, EXPR right)
{
    TYPETAG required;
    int compatible = require_type_conversion(left, right, 0, &required);
    
    EXPR modifiedLeft = left;
    EXPR modifiedRight = right;
    
    if (left->expr_tag == E_VAR)
    {
        modifiedLeft = new_expr_cast(CT_LDEREF, left);
    }
    
    if (right->expr_tag == E_VAR)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
    if (compatible == COMPLETELY_INCOMPATIBLE)
    {
        error("Illegal conversion between types.");
        ty_print_typetag(modifiedLeft->expr_type);
        ty_print_typetag(modifiedRight->expr_type);
    }
    else if (compatible == CONVERSION_REQUIRED)
    {
        if (modifiedRight->expr_tag == TYFLOAT) modifiedRight = new_expr_cast(CT_SGL_REAL, modifiedRight);
        else if (modifiedRight->expr_tag == TYSIGNEDLONGINT) modifiedRight = new_expr_cast(CT_INT_REAL, modifiedRight);
        
        if (modifiedLeft->expr_tag == TYFLOAT) modifiedRight = new_expr_cast(CT_SGL_REAL, modifiedLeft);
        else if (modifiedLeft->expr_tag == TYSIGNEDLONGINT) modifiedRight = new_expr_cast(CT_INT_REAL, modifiedLeft);
    }
    
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_COMPR;
    newExpr->expr_type = TYSIGNEDCHAR;
	newExpr->u.compr_tag = t;
	newExpr->left = modifiedLeft;
	newExpr->right = modifiedRight;
	if (debug == 1) msg("new_expr_compr: COMPRTAG %i", newExpr->u.compr_tag);

	return newExpr;
}

/* New unary function expression */
EXPR new_expr_unfunc(UNFUNCTAG t, EXPR_LIST rightList)
{
    TYPETAG rightExprType = rightList->base->expr_type;
    TYPETAG superExprType = TYVOID;
    BOOLEAN typeOK = TRUE;
    
    switch (t)
    {
        case UF_CHR:
            typeOK = (rightExprType == TYSIGNEDLONGINT);
            superExprType = TYUNSIGNEDCHAR;
            break;
        case UF_ORD:
            typeOK = isOrdinalType(rightExprType);
            superExprType = TYSIGNEDLONGINT;
            break;
        case UF_SUCC:
            typeOK = isOrdinalType(rightExprType);
            superExprType = rightExprType;
            break;
        case UF_PRED:
            typeOK = isOrdinalType(rightExprType);
            superExprType = rightExprType;
            break;
        default:
            bug("new_expr_unfunc Unknown UNFUNCTAG encountered %d", t);
            break;
    }

    if (!typeOK) { error("Invalid type for function."); ty_print_typetag(rightExprType);}

	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_UNFUNC;
    newExpr->expr_type = superExprType;
	newExpr->right = rightList->base;
	newExpr->u.unfunc_tag = t;
	if (debug == 1) msg("new_expr_unfunc: UNFUNCTAG %i", newExpr->u.unfunc_tag);

	return newExpr;
}

/* New global variable expression */
EXPR new_expr_var(ST_ID id)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

    int block;
    ST_DR record = st_lookup(id, &block);
    
    TYPETAG var_type = TYVOID;
    
    if (!record)
    {
        error("'%s' undeclared.", st_get_id_str(id));
    }
    else
    {
        if (record->tag != GDECL)
        {
            error("'%s' is not a globally-defined variable.", st_get_id_str(id));
        }
        else
        {
            var_type = ty_query(record->u.decl.type);
        }
    }

	newExpr->expr_tag = E_VAR;
    newExpr->expr_type = var_type;
	newExpr->u.var_id = id;
	if (debug == 1) msg("new_expr_var");

	return newExpr;
}

EXPR new_expr_cast(CASTTAG t, EXPR right)
{
    EXPR newExpr = (EXPR) malloc(sizeof(expression));
    
    newExpr->expr_tag = E_CAST;
    newExpr->expr_type = right->expr_type;
    newExpr->right = right;
    newExpr->u.cast_tag = t;
    
    if (debug == 1) msg("new_expr_cast: CASTTAG %d", t);
    
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

int require_type_conversion(EXPR left, EXPR right, int precedence, TYPETAG *required)
{
    TYPETAG typeLeft = left->expr_type;
    TYPETAG typeRight = right->expr_type;
    
    if (typeLeft == typeRight) { return COMPLETELY_COMPATIBLE; }
    
    switch (precedence)
    {
        case -1: // Left precedence
            {
                if ((typeLeft == TYDOUBLE && typeRight == TYFLOAT) || (typeLeft == TYDOUBLE && typeRight == TYSIGNEDLONGINT))
                {
                    *required = TYDOUBLE;
                    return CONVERSION_REQUIRED;
                }
                else if ((typeLeft == TYFLOAT && typeRight == TYDOUBLE))
                {
                    *required = TYFLOAT;
                    return CONVERSION_REQUIRED;
                }
            }
            break;
        case 1: // Right precedence
            {
                if ((typeLeft == TYFLOAT && typeRight == TYDOUBLE) || (typeLeft == TYSIGNEDLONGINT && typeRight == TYDOUBLE))
                {
                    *required = TYDOUBLE;
                    return CONVERSION_REQUIRED;
                }
                else if ((typeLeft == TYDOUBLE && typeRight == TYFLOAT))
                {
                    *required = TYFLOAT;
                    return CONVERSION_REQUIRED;
                }
            }
            break;
        case 0: // No precedence
            {
                if ((typeLeft == TYDOUBLE && typeRight == TYFLOAT) || (typeLeft == TYDOUBLE && typeRight == TYSIGNEDLONGINT))
                {
                    *required = TYDOUBLE;
                    return CONVERSION_REQUIRED;
                }
                else if ((typeLeft == TYFLOAT && typeRight == TYDOUBLE) || (typeLeft == TYSIGNEDLONGINT && typeRight == TYDOUBLE))
                {
                    *required = TYDOUBLE;
                    return CONVERSION_REQUIRED;
                }
            }
            break;
        default:
            bug("require_type_conversion Unknown precedence specifier encountered %d", precedence);
            break;
    }
    
    return COMPLETELY_INCOMPATIBLE;
}
