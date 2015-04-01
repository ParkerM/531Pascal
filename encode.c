#include "encode.h"

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
