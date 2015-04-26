#include "encode.h"

void encode_arith_expr(EXPR expr);
void encode_assn_expr(EXPR expr);
void encode_cast_expr(EXPR expr);
void encode_compare_expr(EXPR expr);
void encode_signed_expr(EXPR expr);
void encode_unary_func_expr(EXPR expr);
void encode_variable_expr(EXPR expr);
void encode_function_call(EXPR expr);
void encode_array(EXPR expr);

void encode_successor_func(EXPR expr);
void encode_predecessor_func(EXPR expr);

int get_idx_list_size(INDEX_LIST list);
int get_expr_list_size(EXPR_LIST list);

void encode(ST_ID id)
{
  int blockNum = 0;
  ST_DR record = st_lookup(id, &blockNum);
  if(record != NULL)
  {
    char* idStr = st_get_id_str(id);
    switch(record->tag)
    {
      case GDECL:
      case LDECL:
        b_global_decl(idStr, get_type_alignment(record->u.decl.type), get_type_size(record->u.decl.type));
        encode_decl_from_type(record->u.decl.type);
      break;
      
      case PDECL:
        encode_decl_from_type(record->u.decl.type);
      break;
      
      case TYPENAME:
        encode_decl_from_type(record->u.typename.type);
      break;
      
      default:
        bug("Unknown type: %d", record->tag);
      break;
    }
  }
  else
  {
    fatal("Record could not be found in symbol table");
  }
}

void encode_decl_from_type(TYPE type)
{
  int size = get_type_size(type);
  b_skip(size);
}

int get_type_size(TYPE type)
{
  TYPETAG query = ty_query(type);
  switch(query)
  {
    case TYARRAY:
    {
      INDEX_LIST indices = NULL;
      TYPE elementType = ty_query_array(type, &indices);
      
      long runningProduct = 1L;
      
      while (indices)
      {
        long low, high;
        ty_query_subrange(indices->type, &low, &high);
        runningProduct *= (high-low+1);
        
        indices = indices->next;
      }
      
      return get_type_size(elementType)*runningProduct;
    }
    break;
    
    case TYPTR:
      return 4;
    break;
    
    case TYSIGNEDCHAR:
      return 1;
    break;

    case TYUNSIGNEDINT:
      return 4;
    break;
    
    case TYSIGNEDLONGINT:
      return 4;
    break;
    
    case TYUNSIGNEDCHAR:
      return 1;
    break;
    
    case TYFLOAT:
      return 4;
    break;
    
    case TYDOUBLE:
      return 8;
    break;
    
    case TYSUBRANGE:
    {
      long low, high;
      TYPE base = ty_query_subrange(type, &low, &high);
      
      return get_type_size(base);
    }
    break;
    
    default:
      msg("TYPETAG may have undefined size: ");
      ty_print_typetag(query);
      return 0;
    break;
  }
}

int get_type_alignment(TYPE type)
{
  TYPETAG query = ty_query(type);
  switch(query)
  {
    case TYARRAY:
    {
      INDEX_LIST indices = NULL;
      TYPE elementType = ty_query_array(type, &indices);
      return get_type_alignment(elementType);
    }
    break;
    
    case TYPTR:
      return 4;
    break;
    
    case TYSIGNEDCHAR:
      return 1;
    break;

    case TYUNSIGNEDINT:
      return 4;
    break;
    
    case TYSIGNEDLONGINT:
      return 4;
    break;
    
    case TYUNSIGNEDCHAR:
      return 1;
    break;
    
    case TYFLOAT:
      return 4;
    break;
    
    case TYDOUBLE:
      return 8;
    break;
    
    case TYSUBRANGE:
    {
      long low, high;
      TYPE base = ty_query_subrange(type, &low, &high);
      
      return get_type_alignment(base);
    }
    break;
    
    default:
      msg("TYPETAG may have undefined alignment: ");
      ty_print_typetag(query);
      return 0;
    break;
  }
}

void start_main()
{
    b_func_prologue("main");
}

void end_main()
{
    b_func_epilogue("main");
}

/* -----=====----- EXPRESSIONS -----=====----- */
void encode_expression(EXPR expr)
{
  switch (expr->expr_tag)
  {
    case E_ASSIGN:
      encode_assn_expr(expr);
      break;
    case E_ARITH:
      encode_arith_expr(expr);
      break;
    case E_SIGN:
      encode_signed_expr(expr);
      break;
    case E_INTCONST:
      b_push_const_int(expr->u.integer);
      break;
    case E_REALCONST:
      b_push_const_double(expr->u.real);
      break;
    case E_CHARCONST:
      b_push_const_int(expr->u.character);
      b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
      break;
    case E_BOOLCONST:
      b_push_const_int(expr->u.bool);
      b_convert(TYSIGNEDLONGINT, TYSIGNEDCHAR);
      break;
    case E_COMPR:
      encode_compare_expr(expr);
      break;
    case E_UNFUNC:
      encode_unary_func_expr(expr);
      break;
    case E_VAR:
      encode_variable_expr(expr);
      break;
    case E_CAST:
      encode_cast_expr(expr);
      break;
    case E_FUNC:
      encode_function_call(expr);
      break;
    case E_ARRAY:
      encode_array(expr);
      break;
    default:
      bug("Encountered unknown expression type %d", expr->expr_tag);
      break;
  }
}

void encode_arith_expr(EXPR expr)
{
  encode_expression(expr->left);
  encode_expression(expr->right);
  
  switch (expr->u.arith_tag)
  {
    case AR_ADD:
      b_arith_rel_op(B_ADD, expr->expr_typetag);
      break;
    case AR_SUB:
      b_arith_rel_op(B_SUB, expr->expr_typetag);
      break;
    case AR_MULT:
      b_arith_rel_op(B_MULT, expr->expr_typetag);
      break;
    case AR_IDIV:
      if (expr->expr_typetag != TYSIGNEDLONGINT)
      {
        fatal("An integer division expression is not of type Integer!");
      }
      else
      {
        b_arith_rel_op(B_DIV, TYSIGNEDLONGINT);
      }
      break;
    case AR_RDIV:
      if (expr->expr_typetag != TYDOUBLE || expr->expr_typetag != TYFLOAT)
      {
        fatal("A division expression is not of type Real or Single!");
      }
      else
      {
        b_arith_rel_op(B_DIV, expr->expr_typetag);
      }
      break;
    case AR_MOD:
      b_arith_rel_op(B_MOD, expr->expr_typetag);
      break;
    default:
      error("Unknown ARITH TAG encountered.");
      break;
  }
}

void encode_assn_expr(EXPR expr)
{
  encode_expression(expr->left);
  encode_expression(expr->right);
  
  b_assign(expr->expr_typetag);
  
  b_pop();
}

void encode_cast_expr(EXPR expr)
{
  encode_expression(expr->right);
  
  switch (expr->u.cast_tag)
  {
    case CT_SGL_REAL:
      b_convert(TYFLOAT, TYDOUBLE);
      break;
    case CT_REAL_SGL:
      b_convert(TYDOUBLE, TYFLOAT);
      break;
    case CT_INT_REAL:
      b_convert(TYSIGNEDLONGINT, TYDOUBLE);
      break;
    case CT_INT_SGL:
      b_convert(TYSIGNEDLONGINT, TYFLOAT);
      break;
    case CT_CHAR_INT:
    	b_convert(TYUNSIGNEDCHAR, TYSIGNEDLONGINT);
	    break;
    case CT_LDEREF:
      b_deref(expr->expr_typetag);
      break;
    default:
      bug("Unknown CAST TAG encountered.");
      break;
  }
}

void encode_compare_expr(EXPR expr)
{
  if (expr->expr_typetag != TYSIGNEDCHAR)
  {
    fatal("Boolean expression is not of type TYSIGNEDCHAR.");
  }
  
  TYPETAG argType = expr->left->expr_typetag;
  
  encode_expression(expr->left);
  
  // Convert boolean and characters to integers, since that is what arith_rel_op expects.
  if (argType == TYUNSIGNEDCHAR || argType == TYSIGNEDCHAR)
  {
    b_convert(argType, TYSIGNEDLONGINT);
  }
  
  encode_expression(expr->right);
  
  // Convert boolean and characters to integers, since that is what arith_rel_op expects.
  if (argType == TYUNSIGNEDCHAR || argType == TYSIGNEDCHAR)
  {
    b_convert(argType, TYSIGNEDLONGINT);
    argType = TYSIGNEDLONGINT;
  }
  
  switch (expr->u.compr_tag)
  {
    case CM_EQUAL:
      b_arith_rel_op(B_EQ, argType);
      break;
    case CM_NEQUAL:
      b_arith_rel_op(B_NE, argType);
      break;
    case CM_LESS:
      b_arith_rel_op(B_LT, argType);
      break;
    case CM_GTEQL:
      b_arith_rel_op(B_GE, argType);
      break;
    case CM_GREAT:
      b_arith_rel_op(B_GT, argType);
      break;
    case CM_LSEQL:
      b_arith_rel_op(B_LE, argType);
      break;
    default:
      bug("Unknown COMPR TAG encountered.");
      break;
  }
  
  b_convert(TYSIGNEDLONGINT, TYSIGNEDCHAR);
}

void encode_signed_expr(EXPR expr)
{
  encode_expression(expr->right);
  
  switch (expr->u.sign_tag)
  {
    case SI_PLUS:
      /* No op */
      break;
    case SI_MINUS:
      b_negate(expr->expr_typetag);
      break;
    default:
      bug("Unknown SIGN TAG encountered.");
  }
}

void encode_unary_func_expr(EXPR expr)
{
  switch (expr->u.unfunc_tag)
  {
    case UF_ORD:
      encode_expression(expr->right);
      
      if (expr->right->expr_tag == E_VAR)
      {
        b_deref(expr->right->expr_typetag);
      }
      
      if (expr->right->expr_typetag != TYSIGNEDLONGINT)
      {
        b_convert(expr->right->expr_typetag, TYSIGNEDLONGINT);
      }
      break;
    case UF_CHR:
      encode_expression(expr->right);
      
      if (expr->right->expr_tag == E_VAR)
      {
        b_deref(expr->right->expr_typetag);
      }
      
      b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
      break;
    case UF_SUCC:
      encode_successor_func(expr->right);
      break;
    case UF_PRED:
      encode_predecessor_func(expr->right);
      break;
  } 
}

void encode_variable_expr(EXPR expr)
{
  char* gbl_var_id = st_get_id_str(expr->u.var_func_array.var_id);
  
  b_push_ext_addr(gbl_var_id);
}

void encode_function_call(EXPR expr)
{
  ST_ID func_id = expr->u.var_func_array.var_id;
  char* func_name = st_get_id_str(func_id);
  
  EXPR_LIST arguments = expr->u.var_func_array.arguments;
  
  int sum = 0;
  while (arguments != NULL && arguments->base != NULL)
  {
    TYPE t = ty_build_basic(arguments->base->expr_typetag);
    sum += get_type_size(t);
    arguments = arguments->next;
  }
  
  b_alloc_arglist(sum);
  
  while (arguments != NULL && arguments->base != NULL)
  {
    encode_expression(arguments->base);
    b_load_arg(arguments->base->expr_typetag);
    arguments = arguments->next;
  }
  
  b_funcall_by_name(func_name, expr->expr_typetag);
}

void encode_array(EXPR expr)
{
    TYPE arrayType = expr->expr_fulltype;
    INDEX_LIST allIndices;
    EXPR_LIST indexExprs = expr->u.var_func_array.arguments;

    if (expr->u.var_func_array.array_base_function)
    {
        encode_function_call(expr->right);
    }
    else
    {
        char *array_var = st_get_id_str(expr->u.var_func_array.var_id);
        b_push_ext_addr(array_var);
    }
    
    INDEX_LIST copyOfIndices;
    TYPE arrayElemType = ty_query_array(arrayType, &allIndices);

    while (ty_query(arrayElemType) == TYARRAY)
    {
        INDEX_LIST moreIndices;
        arrayElemType = ty_query_array(arrayElemType, &moreIndices);
        INDEX_LIST currentIndex = allIndices;

        while(currentIndex->next)
        {
            currentIndex = currentIndex->next;
            if (currentIndex == moreIndices)
            {
                break;
            }
        }

        if (currentIndex != moreIndices)
        {
            currentIndex->next = moreIndices;
        }
    }

    int idx_size = get_idx_list_size(allIndices);
    int expr_size = get_expr_list_size(indexExprs);

    if (idx_size != expr_size)
    {
        error("Indices do not match array dimensions. Expected %d %s but %d %s given.", idx_size, (idx_size == 1) ? "index" : "indices", expr_size, (expr_size == 1) ? "was" : "were");
        return;
    }
    
    int  numberElements[idx_size], lowBounds[idx_size];
    EXPR expressions[idx_size];
    
    int idx = 0;
    
    INDEX_LIST indices = allIndices;
    EXPR_LIST exprs = indexExprs;
    
    while (indices)
    {
        long low, high;
        ty_query_subrange(indices->type, &low, &high);
        
        numberElements[idx] = (int)(high-low);
        lowBounds[idx] = (int)low;
        expressions[idx] = exprs->base;
        
        idx++;
        indices = indices->next;
        exprs = exprs->next;
    }
    
    int arrayElemSize = get_type_size(arrayElemType);
    int sizeOfLower = arrayElemSize;
    
    b_push_const_int(0);
    
    for (idx = idx_size-1; idx >= 0; idx--)
    {
        encode_expression(expressions[idx]);
        b_push_const_int(lowBounds[idx]);
        b_arith_rel_op(B_SUB, TYSIGNEDLONGINT);
        b_push_const_int(sizeOfLower);
        b_arith_rel_op(B_MULT, TYSIGNEDLONGINT);
        b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
        
        sizeOfLower = sizeOfLower * numberElements[idx];
    }
    
    b_ptr_arith_op(B_ADD, TYSIGNEDLONGINT, arrayElemSize);
}

void encode_successor_func(EXPR child_expr)
{
  switch (child_expr->expr_tag)
  {
    case E_VAR:
      {
        encode_expression(child_expr);
        b_deref(child_expr->expr_typetag);
        if (child_expr->expr_typetag == TYUNSIGNEDCHAR)
        {
          b_convert(TYUNSIGNEDCHAR, TYSIGNEDLONGINT);
          b_push_const_int(1);
          b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
          b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
        }
        else
        {
          b_push_const_int(1);
          b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
        }
      }
      break;
    case E_CHARCONST:
      {
        encode_expression(child_expr);
        b_convert(TYUNSIGNEDCHAR, TYSIGNEDLONGINT);
        b_push_const_int(1);
        b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
        b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
      }
      break;
    case E_INTCONST:
      {
        b_push_const_int(child_expr->u.integer + 1);
      }
      break;
    default:
      bug("Successor needs an ordinal type!");
  }
}

void encode_predecessor_func(EXPR child_expr)
{
  switch (child_expr->expr_tag)
  {
    case E_FUNC:
    case E_UNFUNC:
      {
        encode_expression(child_expr);
        if (child_expr->expr_typetag == TYUNSIGNEDCHAR)
        {
          b_convert(TYUNSIGNEDCHAR, TYSIGNEDLONGINT);
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
          b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
        }
        else
        {
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
        }
      }
      break;
    case E_VAR:
      {
        encode_expression(child_expr);
        b_deref(child_expr->expr_typetag);
        if (child_expr->expr_typetag == TYUNSIGNEDCHAR)
        {
          b_convert(TYUNSIGNEDCHAR, TYSIGNEDLONGINT);
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
          b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
        }
        else
        {
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
        }
      }
      break;
    case E_CHARCONST:
      {
        encode_expression(child_expr);
        b_convert(TYUNSIGNEDCHAR, TYSIGNEDLONGINT);
        b_push_const_int(-1);
        b_arith_rel_op(B_ADD, TYSIGNEDLONGINT);
        b_convert(TYSIGNEDLONGINT, TYUNSIGNEDCHAR);
      }
      break;
    case E_INTCONST:
      {
        b_push_const_int(child_expr->u.integer - 1);
      }
      break;
    default:
      bug("Predecessor needs an ordinal type!");
  }
}

int get_idx_list_size(INDEX_LIST list)
{
  INDEX_LIST currentItem = list;
  int size = 0;
  
  while (currentItem)
  {
    currentItem = currentItem->next;
    size++;
  }
  
  return size;
}

int get_expr_list_size(EXPR_LIST list)
{
  EXPR_LIST currentItem = list;
  int size = 0;
  
  while (currentItem)
  {
    currentItem = currentItem->next;
    size++;
  }
  
  return size;
}
