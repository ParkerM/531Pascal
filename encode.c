#include "encode.h"

// Directives that allow the type tags herein to match the Pascal types more closely.
#define TYBOOL    TYSIGNEDCHAR
#define TYCHAR    TYUNSIGNEDCHAR
#define TYINTEGER TYSIGNEDLONGINT
#define TYSINGLE  TYFLOAT

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

char last_loop_label[32];

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
    
    case TYBOOL:
      return 1;
    break;

    case TYUNSIGNEDINT:
      return 4;
    break;
    
    case TYINTEGER:
      return 4;
    break;
    
    case TYCHAR:
      return 1;
    break;
    
    case TYSINGLE:
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
      //ty_print_typetag(query);
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
    
    case TYBOOL:
      return 1;
    break;

    case TYUNSIGNEDINT:
      return 4;
    break;
    
    case TYINTEGER:
      return 4;
    break;
    
    case TYCHAR:
      return 1;
    break;
    
    case TYSINGLE:
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
      //ty_print_typetag(query);
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
  if (expr->expr_typetag == TYERROR) { return; }

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
      b_convert(TYINTEGER, TYCHAR);
      break;
    case E_BOOLCONST:
      b_push_const_int(expr->u.bool);
      b_convert(TYINTEGER, TYBOOL);
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
      if (expr->expr_typetag != TYINTEGER)
      {
        fatal("An integer division expression is not of type Integer!");
      }
      else
      {
        b_arith_rel_op(B_DIV, TYINTEGER);
      }
      break;
    case AR_RDIV:
      if (expr->expr_typetag != TYDOUBLE || expr->expr_typetag != TYSINGLE)
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
  
  switch (expr->expr_typetag)
  {
    case TYBOOL:
    case TYCHAR:
    case TYINTEGER:
    case TYPTR:
    case TYSINGLE:
    case TYDOUBLE:
      b_assign(expr->expr_typetag);
      b_pop();
      break;
    default:
      break;
  }
}

void encode_cast_expr(EXPR expr)
{
  encode_expression(expr->right);
  
  switch (expr->u.cast_tag)
  {
    case CT_SGL_REAL:
      b_convert(TYSINGLE, TYDOUBLE);
      break;
    case CT_REAL_SGL:
      b_convert(TYDOUBLE, TYSINGLE);
      break;
    case CT_INT_REAL:
      b_convert(TYINTEGER, TYDOUBLE);
      break;
    case CT_INT_SGL:
      b_convert(TYINTEGER, TYSINGLE);
      break;
    case CT_CHAR_INT:
    	b_convert(TYCHAR, TYINTEGER);
	    break;
    case CT_LDEREF:
      switch (expr->expr_typetag)
      {
        case TYBOOL:
        case TYCHAR:
        case TYINTEGER:
        case TYPTR:
        case TYSINGLE:
        case TYDOUBLE:
          b_deref(expr->expr_typetag);
          break;
        default:
          break;
      }
      break;
    default:
      bug("Unknown CAST TAG encountered.");
      break;
  }
}

void encode_compare_expr(EXPR expr)
{
  if (expr->expr_typetag != TYBOOL)
  {
    fatal("Boolean expression is not a boolean type.");
  }
  
  TYPETAG argType = expr->left->expr_typetag;
  
  encode_expression(expr->left);
  
  // Convert boolean and characters to integers, since that is what arith_rel_op expects.
  if (argType == TYCHAR || argType == TYBOOL)
  {
    b_convert(argType, TYINTEGER);
  }
  
  encode_expression(expr->right);
  
  // Convert boolean and characters to integers, since that is what arith_rel_op expects.
  if (argType == TYCHAR || argType == TYBOOL)
  {
    b_convert(argType, TYINTEGER);
    argType = TYINTEGER;
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
  
  b_convert(TYINTEGER, TYBOOL);
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
      
      if (expr->right->expr_typetag != TYINTEGER)
      {
        b_convert(expr->right->expr_typetag, TYINTEGER);
      }
      break;
    case UF_CHR:
      encode_expression(expr->right);
      
      if (expr->right->expr_tag == E_VAR)
      {
        b_deref(expr->right->expr_typetag);
      }
      
      b_convert(TYINTEGER, TYCHAR);
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
    EXPR_LIST indexExprs = expr->u.var_func_array.arguments;

    // First, encode the base address of the array, and retrieve its indexing information.
    encode_expression(expr->right);
    
    INDEX_LIST index_list;
    ty_query_array(expr->right->expr_fulltype, &index_list);
    
    long low, high;
    ty_query_subrange(index_list->type, &low, &high);
    
    // If the index expression is not an integer, complain.
    if (indexExprs->base->expr_typetag != TYINTEGER)
    {
        error("Incompatible index type in array access");
        return;
    }
    
    // Otherwise, encode the index expression and compute the offset from the base, according
    // to the lower limit of the index's subrange of possible index values.
    encode_expression(indexExprs->base);
    
    if (indexExprs->base->expr_tag == E_VAR || indexExprs->base->expr_tag == E_ARRAY)
    {
      b_deref(indexExprs->base->expr_typetag);
    }
    
    b_push_const_int((int)low);
    
    b_arith_rel_op(B_SUB, TYINTEGER);
    
    b_ptr_arith_op(B_ADD, TYINTEGER, get_type_size(expr->expr_fulltype));
}

void encode_successor_func(EXPR child_expr)
{
  switch (child_expr->expr_tag)
  {
    case E_FUNC:
    case E_UNFUNC:
      {
        encode_expression(child_expr);
        if (child_expr->expr_typetag == TYCHAR)
        {
          b_convert(TYCHAR, TYINTEGER);
          b_push_const_int(1);
          b_arith_rel_op(B_ADD, TYINTEGER);
          b_convert(TYINTEGER, TYCHAR);
        }
        else
        {
          b_push_const_int(1);
          b_arith_rel_op(B_ADD, TYINTEGER);
        }
      }
      break;
    case E_VAR:
      {
        encode_expression(child_expr);
        b_deref(child_expr->expr_typetag);
        if (child_expr->expr_typetag == TYCHAR)
        {
          b_convert(TYCHAR, TYINTEGER);
          b_push_const_int(1);
          b_arith_rel_op(B_ADD, TYINTEGER);
          b_convert(TYINTEGER, TYCHAR);
        }
        else
        {
          b_push_const_int(1);
          b_arith_rel_op(B_ADD, TYINTEGER);
        }
      }
      break;
    case E_CHARCONST:
      {
        encode_expression(child_expr);
        b_convert(TYCHAR, TYINTEGER);
        b_push_const_int(1);
        b_arith_rel_op(B_ADD, TYINTEGER);
        b_convert(TYINTEGER, TYCHAR);
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
        if (child_expr->expr_typetag == TYCHAR)
        {
          b_convert(TYCHAR, TYINTEGER);
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYINTEGER);
          b_convert(TYINTEGER, TYCHAR);
        }
        else
        {
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYINTEGER);
        }
      }
      break;
    case E_VAR:
      {
        encode_expression(child_expr);
        b_deref(child_expr->expr_typetag);
        if (child_expr->expr_typetag == TYCHAR)
        {
          b_convert(TYCHAR, TYINTEGER);
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYINTEGER);
          b_convert(TYINTEGER, TYCHAR);
        }
        else
        {
          b_push_const_int(-1);
          b_arith_rel_op(B_ADD, TYINTEGER);
        }
      }
      break;
    case E_CHARCONST:
      {
        encode_expression(child_expr);
        b_convert(TYCHAR, TYINTEGER);
        b_push_const_int(-1);
        b_arith_rel_op(B_ADD, TYINTEGER);
        b_convert(TYINTEGER, TYCHAR);
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

void store_label(char* label) {
  strcpy(last_loop_label, label);
}

char *get_last_label() {
  return last_loop_label;
}
