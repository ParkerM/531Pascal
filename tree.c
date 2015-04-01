/*
 * TREE.C
 *
 * This source file defines the various methods used to create, manipulate, and parse the syntax
 * tree(s) and related structures created during the parsing of the Pascal file.
 *
 * Author(s): Michael Brunson
 *            John Mauney
 *            Myndert Papenhuyzen
 *
 *      Date: March 29, 2015
 *
 *   Purpose: CSCE 531 (Compiler Construction) Project
 */
 
#include "tree.h"

TYPE_LIST unresolveds = NULL;

void add_unresolved_ptr_to_list(TYPE ptr);

num_const_p allocate_number_const_int(long i)
{
  num_const_p number = (num_const_p) malloc(sizeof(num_const));
  
  number->type = TYSIGNEDLONGINT;
  number->v.integer = i;
  
  return number;
}

num_const_p allocate_number_const_real(double d)
{
  num_const_p number = (num_const_p) malloc(sizeof(num_const));
  
  number->type = TYDOUBLE;
  number->v.real = d;
  
  return number;
}

num_const_p sign_constant(int i, num_const_p number)
{
  if (i != -1 || i != 1)
  {
    // not good!
  }
  else
  {
    switch (number->type)
    {
      case TYSIGNEDLONGINT:
        number->v.integer *= i;
        break;
      case TYDOUBLE:
        number->v.real *= (double)i;
        break;
      default:
        break;
        // not good!
    }
  }
  
  return number;
}
 
stid_list new_stid_list(ST_ID root)
{
  //message("new stid list");
  // Allocate a new stid_item_p value.
  stid_item_p value = (stid_item_p) malloc(sizeof(STID_ITEM));
  
  // Explicitly set the next pointer to NULL and set the enrollment_papers.
  value->next = NULL;
  value->enrollment_papers = root;
  
  return value;
}

void append_stid_to_list(stid_list base, ST_ID new_id)
{
  //message("append stid to list");

  // Allocate a new stid_item_p item.
  stid_item_p newItem = (stid_item_p) malloc(sizeof(STID_ITEM));
  
  // Explicitly set the next pointer to NULL and set the enrollment_papers.
  newItem->next = NULL;
  newItem->enrollment_papers = new_id;
  
  // Now, loop through the list until the last item is found.
  stid_item_p currentItem = base;
  
  while (currentItem->next)
  {
      currentItem = currentItem->next;
  }
  
  // Append the newItem to the end of the list.
  currentItem->next = newItem;
  
}

typedef_item_p make_typedef_node(ST_ID id, TYPE t)
{
  //message("make typedef node");
  typedef_item_p new_typedef = (typedef_item_p) malloc(sizeof(TYPEDEF_ITEM));
  
  new_typedef->new_def = id;
  new_typedef->old_type = t;
  
  return new_typedef;
}
 
void install_typedef(typedef_item_p item)
{
    //message("install typedef");
    
    ST_ID id = item->new_def;
    TYPE t = item->old_type;
    
    if (t != NULL)
    {
      ST_DR rec = stdr_alloc();
      rec->tag = TYPENAME;
      rec->u.typename.type = t;
      
      BOOLEAN didItWork = st_install(id, rec);
      
      if (!didItWork)
      {
        error("Found duplicate definition for \"%s\"", sc_line(), st_get_id_str(id));
      }
    }
}

void process_unresolved_types()
{
    //message("process unresolved types");
    
    if (unresolveds != NULL)
    {
      BOOLEAN anyStillUnknown = FALSE;
      
      TYPE_LIST current_item = unresolveds;
      
      while (current_item)
      {
        ST_ID id;
        ty_query_ptr(current_item->type, &id);
        
        int ignore = 0;
        ST_DR aRecord = st_lookup(id, &ignore);
        
        if (aRecord != NULL)
        {
          if (aRecord->tag == TYPENAME)
          {
            TYPE theType = aRecord->u.typename.type;
            
            ty_resolve_ptr(current_item->type, theType);
          }
        }
        else
        {
          error("Unresolved pointer %s", st_get_id_str(id));
          anyStillUnknown = TRUE;
        }
        
        current_item = current_item->next;
      }
    }
}

TYPE get_basic_type(char* typename)
{
  if (strcmp(typename, "Boolean") == 0)
  {
    return ty_build_basic(TYSIGNEDCHAR);
  }
  if (strcmp(typename, "Integer") == 0)
  {
    return ty_build_basic(TYSIGNEDLONGINT);
  }
  if (strcmp(typename, "Char") == 0)
  {
    return ty_build_basic(TYUNSIGNEDCHAR);
  }
  if (strcmp(typename, "Single") == 0)
  {
    return ty_build_basic(TYFLOAT);
  }
  if (strcmp(typename, "Real") == 0)
  {
    return ty_build_basic(TYDOUBLE);
  }
  else
  {
    ST_ID identifier = st_enter_id(typename);
    
    int ignore = 0;
    ST_DR isItThere = st_lookup(identifier, &ignore);
    
    if (isItThere)
    {
      if (isItThere->tag == TYPENAME)
      {
        TYPE theType = isItThere->u.typename.type;
        
        return theType;
      }
      else
      {
        error("Trying to redefine non-type identifier.");
        /* TODO: Keep track of semantic errors. */
        return ty_build_basic(TYERROR);
      }
    }
    else
    {
      TYPE unresolvedPtr = ty_build_unresolved_ptr(identifier);
    
      add_unresolved_ptr_to_list(unresolvedPtr);
    
      return unresolvedPtr;
    }
  }
}

TYPE create_subrange(num_const_p low, num_const_p high)
{
  if (low->type != TYSIGNEDLONGINT && high->type != TYSIGNEDLONGINT)
  {
    // not good!
    return NULL;
  }
  else
  {
    long low_val = low->v.integer;
    long high_val = high->v.integer;
    
    TYPE intType = ty_build_basic(TYSIGNEDLONGINT);
    
    TYPE subrange = ty_build_subrange(intType, low_val, high_val);
    
    return subrange;
  }
}

BOOLEAN isSimpleType(TYPE t)
{
  TYPETAG tag = ty_query(t);
  
  switch (tag)
  {
    case TYFLOAT:
      return TRUE;
    break;
    
    case TYDOUBLE:
      return TRUE;
    break;
    
    case TYLONGDOUBLE:
      return TRUE;
    break;
    
    case TYSIGNEDLONGINT:
      return TRUE;
    break;
    
    case TYSIGNEDSHORTINT:
      return TRUE;
    break;
    
    case TYSIGNEDINT:
      return TRUE;
    break;
    
    case TYUNSIGNEDLONGINT:
      return TRUE;
    break;
    
    case TYUNSIGNEDSHORTINT:
      return TRUE;
    break;
    
    case TYUNSIGNEDINT:
      return TRUE; 
    break;
    
    case TYUNSIGNEDCHAR:
      return TRUE; 
    break; 
    
    case TYSIGNEDCHAR:
      return TRUE; 
    break;
    
    case TYPTR:
    {
      ST_ID ignore;
      TYPE pointsTo = ty_query_ptr(t, &ignore);
      
      return (pointsTo != NULL);
    }
    break;
    
    default:
      return FALSE;
    break;
  }
}

BOOLEAN isDataType(TYPE t)
{
  if (isSimpleType(t))
  {
    return TRUE;
  }
  
  TYPETAG tag = ty_query(t);
  
  switch (tag)
  {
    case TYFUNC:
      return FALSE;
    break;
    
    case TYVOID:
      return FALSE;
    break;
    
    case TYERROR:
      return FALSE;
    break;
    
    case TYPTR:
    {
      ST_ID ignore;
      TYPE pointsTo = ty_query_ptr(t, &ignore);
      
      return (pointsTo != NULL);
    }
    break;
    
    default:
      return TRUE;
    break;
  }
}

void paramdec(stid_list list, TYPE t)
{
   message("paramdec");
}

PARAM_LIST make_new_param_list(PARAM_LIST t)
{
   message("make new param list");
}

PARAM_LIST make_new_param_list_from_ids(stid_item_p list)
{
}

PARAM_LIST append_to_param_list(PARAM_LIST list, PARAM_LIST t)
{
   message("append to param list");
}

PARAM_LIST merge_param_lists(PARAM_LIST list1, PARAM_LIST list2)
{
    message("merge param lists");
}

TYPE_LIST make_new_type_list(TYPE t)
{
    //message("make new type list");
    // Allocate a new TYPE_LIST value.
    TYPE_LIST value = (TYPE_LIST) malloc(sizeof(TLIST_NODE));
    
    // Explicitly set the next pointer to NULL and copy the types from item
    value->type = t;
    value->next = NULL;
    value->prev = NULL;
    
    return value;
}

TYPE_LIST append_to_type_list(TYPE_LIST list, TYPE t)
{
    //message("append to type list");

    TYPE_LIST newNode = (TYPE_LIST) malloc(sizeof(TLIST_NODE));
    newNode->type = t;

    TYPE_LIST current = list;

    while (current->next) 
    {
        current = current->next;
    }

    current->next = newNode;
    
    return current;
}

void vardec(stid_list list, TYPE t)
{
    //message("vardec");

    //create ST data record
    ST_DR dr;

    //while list is not null
    while (list)
    {
        dr = stdr_alloc();
        dr->tag = GDECL;
        dr->u.decl.type = t;
        
        BOOLEAN newRec = st_install(list->enrollment_papers, dr);
        
        if (newRec)
        {
          encode(list->enrollment_papers);
        }
        else
        {
          error("Duplicate definition at \"%s\"", st_get_id_str(list->enrollment_papers));
        }
        //message("added type: ");
        //ty_print_type(t); //currently gives "illegal typetag"
        
        
        
        list = list->next;
    }
}

// ---

void add_unresolved_ptr_to_list(TYPE ptr)
{
  if (unresolveds == NULL)
  {
    unresolveds = (TYPE_LIST) malloc(sizeof(TLIST_NODE));
    
    unresolveds->type = ptr;
    unresolveds->next = NULL;
    unresolveds->prev = NULL;
  }
  else
  {
    TYPE_LIST new_node = (TYPE_LIST) malloc(sizeof(TLIST_NODE));
    
    new_node->type = ptr;
    new_node->next = NULL;
    new_node->prev = NULL;
    
    TYPE_LIST current = unresolveds;
    
    while (current->next) { current = current->next; }
    
    new_node->prev = current;
    current->next = new_node;
  }
}
