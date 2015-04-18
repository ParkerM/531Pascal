#include "encode.h"

void encode_arith_expr(EXPR expr);
void encode_assn_expr(EXPR expr);
void encode_cast_expr(EXPR expr);
void encode_compare_expr(EXPR expr);
void encode_signed_expr(EXPR expr);
void encode_unary_func_expr(EXPR expr);
void encode_variable_expr(EXPR expr);

void encode_successor_func(EXPR expr);
void encode_predecessor_func(EXPR expr);


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
      b_arith_rel_op(B_ADD, expr->expr_type);
      break;
    case AR_SUB:
      b_arith_rel_op(B_SUB, expr->expr_type);
      break;
    case AR_MULT:
      b_arith_rel_op(B_MULT, expr->expr_type);
      break;
    case AR_IDIV:
      if (expr->expr_type != TYSIGNEDLONGINT)
      {
        fatal("An integer division expression is not of type Integer!");
      }
      else
      {
        b_arith_rel_op(B_DIV, TYSIGNEDLONGINT);
      }
      break;
    case AR_RDIV:
      if (expr->expr_type != TYDOUBLE || expr->expr_type != TYFLOAT)
      {
        fatal("A division expression is not of type Real or Single!");
      }
      else
      {
        b_arith_rel_op(B_DIV, expr->expr_type);
      }
      break;
    case AR_MOD:
      b_arith_rel_op(B_MOD, expr->expr_type);
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
  
  b_assign(expr->expr_type);
  
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
    case CT_LDEREF:
      b_deref(expr->expr_type);
      break;
    default:
      bug("Unknown CAST TAG encountered.");
      break;
  }
}

void encode_compare_expr(EXPR expr)
{
  if (expr->expr_type != TYSIGNEDCHAR)
  {
    fatal("Boolean expression is not of type TYSIGNEDCHAR.");
  }
  
  encode_expression(expr->left);
  encode_expression(expr->right);
  
  switch (expr->u.compr_tag)
  {
    case CM_EQUAL:
      b_arith_rel_op(B_EQ, TYSIGNEDCHAR);
      break;
    case CM_NEQUAL:
      b_arith_rel_op(B_NE, TYSIGNEDCHAR);
      break;
    case CM_LESS:
      b_arith_rel_op(B_LT, TYSIGNEDCHAR);
      break;
    case CM_GTEQL:
      b_arith_rel_op(B_GE, TYSIGNEDCHAR);
      break;
    case CM_GREAT:
      b_arith_rel_op(B_GT, TYSIGNEDCHAR);
      break;
    case CM_LSEQL:
      b_arith_rel_op(B_LE, TYSIGNEDCHAR);
      break;
    default:
      bug("Unknown COMPR TAG encountered.");
      break;
  }
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
      b_negate(expr->expr_type);
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
      
      if (expr->right->expr_type != TYSIGNEDLONGINT)
      {
        b_convert(expr->right->expr_type, TYSIGNEDLONGINT);
      }
      break;
    case UF_CHR:
      encode_expression(expr->right);
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
  char* gbl_var_id = st_get_id_str(expr->u.var_id);
  
  b_push_ext_addr(gbl_var_id);
}

void encode_successor_func(EXPR child_expr)
{
  switch (child_expr->expr_tag)
  {
    case E_VAR:
      {
        encode_expression(child_expr);
        b_deref(child_expr->expr_type);
        if (child_expr->expr_type == TYUNSIGNEDCHAR)
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
    case E_VAR:
      {
        encode_expression(child_expr);
        b_deref(child_expr->expr_type);
        if (child_expr->expr_type == TYUNSIGNEDCHAR)
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
