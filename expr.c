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
CASTTAG get_cast_constant(TYPETAG from, TYPETAG to);

int debug = 0; //set to 1 for debug messages

/* New assignment expression */
EXPR new_expr_assign(EXPR left, EXPR right) 
{
    TYPETAG required;
    int compatible = require_type_conversion(left, right, -1, &required);
    
    EXPR modifiedRight = right;
    
    if (right->expr_tag == E_VAR || right->expr_tag == E_ARRAY)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
    if (compatible == COMPLETELY_INCOMPATIBLE)
    {
        error("Illegal conversion between types.");
        ty_print_typetag(left->expr_typetag); msg("");
        ty_print_typetag(modifiedRight->expr_typetag); msg("");
    }
    else if (compatible == CONVERSION_REQUIRED)
    {
        if (modifiedRight->expr_typetag == TYFLOAT && required == TYDOUBLE)
        {
            modifiedRight = new_expr_cast(CT_SGL_REAL, modifiedRight);
        }
        else if (modifiedRight->expr_typetag == TYDOUBLE && required == TYFLOAT)
        {
            modifiedRight = new_expr_cast(CT_REAL_SGL, modifiedRight);
        }
        else if (modifiedRight->expr_typetag == TYSIGNEDLONGINT && required == TYDOUBLE)
        {
            modifiedRight = new_expr_cast(CT_INT_REAL, modifiedRight);
        }
        else if (modifiedRight->expr_typetag == TYSIGNEDLONGINT && required == TYFLOAT)
        {
            modifiedRight = new_expr_cast(CT_INT_SGL, modifiedRight);
        }
    }
    
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_ASSIGN;
	newExpr->expr_typetag = left->expr_typetag;
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
    
    if (left->expr_tag == E_VAR || left->expr_tag == E_ARRAY)
    {
        modifiedLeft = new_expr_cast(CT_LDEREF, left);
    }
    
    if (right->expr_tag == E_VAR || right->expr_tag == E_ARRAY)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
    if (compatible == COMPLETELY_INCOMPATIBLE)
    {
        error("Illegal conversion between types.");
        ty_print_typetag(modifiedLeft->expr_typetag); msg("");
        ty_print_typetag(modifiedRight->expr_typetag); msg("");
    }
    else if (compatible == CONVERSION_REQUIRED)
    {
      if (modifiedLeft->expr_typetag != required)
      {
        CASTTAG tag = get_cast_constant(modifiedLeft->expr_typetag, required);
        modifiedLeft = new_expr_cast(tag, modifiedLeft);
      }
      
      if (modifiedRight->expr_typetag != required)
      {
        CASTTAG tag = get_cast_constant(modifiedRight->expr_typetag, required);
        modifiedRight = new_expr_cast(tag, modifiedRight);
      }
    }
    
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->u.arith_tag = t;
	newExpr->expr_tag = E_ARITH;
	newExpr->expr_typetag = modifiedLeft->expr_typetag;
	newExpr->left = modifiedLeft;
	newExpr->right = modifiedRight;
	if (debug == 1) msg("new_expr_arith: ARITHTAG %i", newExpr->u.arith_tag);

	return newExpr;
}

/* New signed expression */
EXPR new_expr_sign(int sign, EXPR right)
{
    EXPR modifiedRight = right;
    
    if (right->expr_tag == E_VAR || right->expr_tag == E_ARRAY)
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
    newExpr->expr_typetag = modifiedRight->expr_typetag;
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
    newExpr->expr_typetag = TYSIGNEDLONGINT;
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
    newExpr->expr_typetag = TYDOUBLE;
	newExpr->u.real = d;
	if (debug == 1) msg("new_expr_realconst %f", newExpr->u.real);

	return newExpr;
}

/* new character constant expression */
EXPR new_expr_strconst(char *str)
{
    EXPR newExpr = (EXPR) malloc(sizeof(expression));
    
    newExpr->expr_tag = E_CHARCONST;
    newExpr->expr_typetag = TYUNSIGNEDCHAR;
    newExpr->u.character = str[0];
    if (debug) msg("new_expr_strconst %s", newExpr->u.character);
    
    return newExpr;
}

/* New boolean constant (i.e. TRUE or FALSE). */
EXPR new_expr_boolconst(int bool)
{
    EXPR newExpr = (EXPR) malloc(sizeof(expression));
    
    newExpr->expr_tag = E_BOOLCONST;
    newExpr->expr_typetag = TYSIGNEDCHAR;
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
    
    if (left->expr_tag == E_VAR || left->expr_tag == E_ARRAY)
    {
        modifiedLeft = new_expr_cast(CT_LDEREF, left);
    }
    
    if (right->expr_tag == E_VAR || right->expr_tag == E_ARRAY)
    {
        modifiedRight = new_expr_cast(CT_LDEREF, right);
    }
    
    if (compatible == COMPLETELY_INCOMPATIBLE)
    {
        error("Illegal conversion between types.");
        ty_print_typetag(modifiedLeft->expr_typetag); error("");
        ty_print_typetag(modifiedRight->expr_typetag); error("");
    }
    else if (compatible == CONVERSION_REQUIRED)
    {
        if (modifiedLeft->expr_typetag != required)
        {
            CASTTAG tag = get_cast_constant(modifiedLeft->expr_typetag, required);
            modifiedLeft = new_expr_cast(tag, modifiedLeft);
        }
        
        if (modifiedRight->expr_typetag != required)
        {
            CASTTAG tag = get_cast_constant(modifiedRight->expr_typetag, required);
            modifiedRight = new_expr_cast(tag, modifiedRight);
        }
    }
    
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

	newExpr->expr_tag = E_COMPR;
    newExpr->expr_typetag = TYSIGNEDCHAR;
	newExpr->u.compr_tag = t;
	newExpr->left = modifiedLeft;
	newExpr->right = modifiedRight;
	if (debug == 1) msg("new_expr_compr: COMPRTAG %i", newExpr->u.compr_tag);

	return newExpr;
}

/* New unary function expression */
EXPR new_expr_unfunc(UNFUNCTAG t, EXPR_LIST rightList)
{
    
    TYPETAG rightExprType = rightList->base->expr_typetag;
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
    newExpr->expr_typetag = superExprType;
	newExpr->right = rightList->base;
	newExpr->u.unfunc_tag = t;
	
	if (debug == 1) msg("new_expr_unfunc: UNFUNCTAG %i", newExpr->u.unfunc_tag);

	return newExpr;
}

/* New global variable or function call expression */
EXPR new_expr_identifier(ST_ID id)
{
	//allocate new EXPR
	EXPR newExpr = (EXPR) malloc(sizeof(expression));

    int block;
    ST_DR record = st_lookup(id, &block);
    
    TYPETAG var_typetag = TYVOID;
    TYPE    var_type = NULL;
    EXPRTAG expr_t = E_VAR;
    
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
            var_type = record->u.decl.type;
            var_typetag = ty_query(var_type);
        }
    }
    
    if (var_typetag == TYFUNC)
    {
        PARAM_LIST params;
        BOOLEAN check;
        var_typetag = ty_query(ty_query_func(var_type, &params, &check));
        expr_t = E_FUNC;
    }
    
    if (var_typetag == TYARRAY)
    {
        INDEX_LIST indices;
        TYPE arrayElemType = ty_query_array(var_type, &indices);
        
        while (ty_query(arrayElemType) == TYARRAY)
        {
            arrayElemType = ty_query_array(arrayElemType, &indices);
        }
        
        var_typetag = ty_query(arrayElemType);
    }
    
    newExpr->expr_tag = expr_t;
    newExpr->expr_typetag = var_typetag;
    newExpr->expr_fulltype = var_type;
    newExpr->u.var_func_array.var_id = id;
	
	if (debug == 1) msg("new_expr_identifier var/func");

	return newExpr;
}

EXPR new_expr_var_funccall(EXPR base, EXPR_LIST arguments)
{
    EXPR toReturn;

    if (base->expr_tag == E_VAR)
    {
        if (arguments) { error("'%s' is a variable, but it's being treated as a function!", base->u.var_func_array.var_id); }
        
        toReturn = base;
    }
    else if (base->expr_tag == E_FUNC)
    {
        toReturn = (EXPR) malloc(sizeof(expression));
        
        int block;
        ST_DR func_rec = st_lookup(base->u.var_func_array.var_id, &block);
        
        PARAM_LIST params;
        BOOLEAN check;
        TYPE func_type = ty_query_func(func_rec->u.decl.type, &params, &check);
        
        int numParams = 0;
        while (params != NULL)
        {
            numParams++;
            params = params->next;
        }
        
        int numArgs = 0;
        EXPR_LIST myArgs = arguments;
        while (myArgs != NULL && myArgs->base != NULL)
        {
            numArgs++;
            myArgs = myArgs->next;
        }
        
        if (numParams != numArgs) { error("Function '%s' expected %d arguments, received %d", st_get_id_str(base->u.var_func_array.var_id), numParams, numArgs); }
        
        toReturn->expr_tag = E_FUNC;
        toReturn->expr_typetag = base->expr_typetag;
        toReturn->u.var_func_array.var_id = base->u.var_func_array.var_id;
        toReturn->u.var_func_array.arguments = arguments;
        
    }
    
    return toReturn;
}

EXPR new_expr_array(EXPR base, EXPR_LIST indices)
{
  // If the base expression is already an array, then append the new indices to the head of the existing ones.
  if (base->expr_tag == E_ARRAY)
  {
    EXPR_LIST oldList = base->u.var_func_array.arguments;
    
    while (oldList->next)
    {
      oldList = oldList->next;
    }
    
    oldList->next = indices;
  }
  // If the base expression is just a variable name, create a new array node.
  else if (base->expr_tag == E_VAR)
  {
    base->expr_tag = E_ARRAY;
    base->u.var_func_array.arguments = indices;
  }
  else if (base->expr_tag == E_FUNC)
  {
    EXPR arrayExpr = (EXPR) malloc(sizeof(expression));
    
    arrayExpr->expr_tag = E_ARRAY;
    arrayExpr->expr_typetag = base->expr_typetag;
    arrayExpr->expr_fulltype = ty_query_func(base->expr_fulltype, NULL, NULL);
    arrayExpr->right = base;
    arrayExpr->u.var_func_array.var_id = base->u.var_func_array.var_id;
    arrayExpr->u.var_func_array.arguments = indices;
    arrayExpr->u.var_func_array.array_base_function = TRUE;
    
    return arrayExpr;
  }
  // Any other base expression type is an error.
  else
  {
    bug("Received inappropriate expression tag to new_expr_array(%d, ...)", base->expr_tag);
  }
  
  return base;
}

EXPR new_expr_cast(CASTTAG t, EXPR right)
{
    EXPR newExpr = (EXPR) malloc(sizeof(expression));
    
    newExpr->expr_tag = E_CAST;
    
    switch (t)
    {
        case CT_LDEREF: newExpr->expr_typetag = right->expr_typetag; break;
        case CT_SGL_REAL: 
        case CT_INT_REAL: newExpr->expr_typetag = TYDOUBLE; break;
        case CT_REAL_SGL: 
        case CT_INT_SGL: newExpr->expr_typetag = TYFLOAT; break;
        default: error("Unknown cast tag encountered, %d", t); break;
    }
    
    newExpr->right = right;
    newExpr->u.cast_tag = t;
    
    if (debug == 1) { msg("new_expr_cast: CASTTAG %d", t); ty_print_typetag(right->expr_typetag); msg(""); }
    
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

    EXPR_LIST listRef = list;
    
    while (listRef->next)
    {
        listRef = listRef->next;
    }

	//put newList at the tail of list and assign the expression
	listRef->next = newList;
    newList->next = NULL;
	newList->base = newItem;
	if (debug == 1) msg("append_to_expr_list");

	return list;
}

int require_type_conversion(EXPR left, EXPR right, int precedence, TYPETAG *required)
{
    TYPETAG typeLeft = left->expr_typetag;
    TYPETAG typeRight = right->expr_typetag;
    
    if (debug)
    {
      msg("require_type_conversion(%d, %d, %d, *)", typeLeft, typeRight, precedence);
      ty_print_typetag(typeLeft); msg("");
      ty_print_typetag(typeRight); msg("");
    }
        
    if (typeLeft == typeRight) { *required = typeLeft; return COMPLETELY_COMPATIBLE; }
    
    switch (precedence)
    {
        case -1: // Left precedence
            {
                if (typeLeft == TYDOUBLE && (typeRight == TYFLOAT || typeRight == TYSIGNEDLONGINT))
                {
                    *required = TYDOUBLE;
                    return CONVERSION_REQUIRED;
                }
                else if (typeLeft == TYFLOAT && (typeRight == TYDOUBLE || typeRight == TYSIGNEDLONGINT))
                {
                    *required = TYFLOAT;
                    return CONVERSION_REQUIRED;
                }
            }
            break;
        case 1: // Right precedence
            {
                if ((typeLeft == TYFLOAT || typeLeft == TYSIGNEDLONGINT) && typeRight == TYDOUBLE)
                {
                    *required = TYDOUBLE;
                    return CONVERSION_REQUIRED;
                }
                else if ((typeLeft == TYDOUBLE || typeLeft == TYSIGNEDLONGINT) && typeRight == TYFLOAT)
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
                else if ((typeLeft == TYFLOAT && typeRight == TYSIGNEDLONGINT) || (typeLeft == TYSIGNEDLONGINT && typeRight == TYFLOAT))
                {
                    *required = TYFLOAT;
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

CASTTAG get_cast_constant(TYPETAG from, TYPETAG to)
{
  if (from == TYFLOAT)
  {
    if (to == TYDOUBLE)
    return CT_SGL_REAL;
    
  }
  else if (from == TYDOUBLE)
  {
    if (to == TYFLOAT)
    return CT_REAL_SGL;
  }
  else if (from == TYSIGNEDLONGINT)
  {
    if (to == TYFLOAT)
    return CT_INT_SGL;
    
    if (to == TYDOUBLE)
    return CT_INT_REAL;
  }
  else
  {
    error("From not recognized ");
    ty_print_typetag(from); msg(" is what is found.");
  }
  return 0;
  
}
