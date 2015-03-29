#include "encode.h"

void encode(ST_ID id)
{
  int blockNum = 0;
  ST_DR record = st_lookup(id, &blockNum);
  char* idStr = st_get_id_str(id);
  switch(record->tag)
  {
    case GDECL:
      b_global_decl(idStr, get_type_alignment(record->u.decl->type), get_type_size(record->u.decl->type));
      encode_decl_from_type(record->u.decl->type);
    break;
    
    case PDECL:
      encode_decl_from_type(record->u.decl->type);
    break;
    
    case TYPENAME:
      encode_decl_from_type(record->u.typename->type);
    break;
    
    default:
      bug("Unknown type: %d", record->tag);
    break;
  }
}

void encode_decl_from_type(TYPE type)
{
  TYPETAG tag = ty_query(type);
  int size = get_type_size(type);
  b_skip(size);
}

int get_type_size(TYPE type)
{
  switch(ty_query(type))
  {
    case TYARRAY:
      INDEX_LIST indices;
      TYPE elementType = ty_query_array(type, &indices);
      long low, high;
      ty_query_subrange(indices, &low, &high);
      return get_type_size(elementType)*(high-low+1);
    break;
    
    case TYPTR:
      return 4;
    break;
    
    case TYSIGNEDCHAR:
      return 1;
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
    
    default:
      return 0;
    break;
  }
}

int get_type_alignment(TYPE type)
{
  switch(ty_query(type))
  {
    case TYARRAY:
      INDEX_LIST indices;
      TYPE elementType = ty_query_array(type, &indices);
      return get_type_alignment(elementType);
    break;
    
    case TYPTR:
      return 4;
    break;
    
    case TYSIGNEDCHAR:
      return 1;
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
    
    default:
      return 0;
    break;
  }
}
