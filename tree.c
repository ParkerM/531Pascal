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
 #include "encode.h"
 #include "message.h"
 #include "types.h"
 #include "symtab.h"
 
 stid_list new_stid_list(ST_ID root)
 {
    message("new stid list");
    // Allocate a new stid_item_p value.
    stid_item_p value = (stid_item_p) malloc(sizeof(STID_ITEM));
    
    // Explicitly set the next pointer to NULL and set the enrollment_papers.
    value->next = NULL;
    value->enrollment_papers = root;
    
    return value;
 }
 
 void append_stid_to_list(stid_list base, ST_ID new_id)
 {
    message("append stid to list");

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
    message("make typedef node");
    typedef_item_p new_typedef = (typedef_item_p) malloc(sizeof(TYPEDEF_ITEM));
    
    new_typedef->next = NULL;
    new_typedef->new_def = id;
    new_typedef->old_type = t;
 }
 
void append_typedef_to_list(typedef_list base, typedef_item_p newItem)
{
    message("append typedef to list");
    typedef_item_p current = base;
    
    while (current->next)
    {
        current = current->next;
    }
    
    current->next = newItem;
}

void process_typedefs(typedef_list list)
{
    message("process typedefs");
}

TYPE get_basic_type(char* typename)
{
  if (strcmp(typename, "boolean") == 0)
  {
    return ty_build_basic(TYSIGNEDCHAR);
  }
  if (strcmp(typename, "integer") == 0)
  {
    return ty_build_basic(TYSIGNEDINT);
  }
  if (strcmp(typename, "char") == 0)
  {
    return ty_build_basic(TYUNSIGNEDCHAR);
  }
  if (strcmp(typename, "single") == 0)
  {
    return ty_build_basic(TYFLOAT);
  }
  if (strcmp(typename, "real") == 0)
  {
    return ty_build_basic(TYDOUBLE);
  }
  else
  {
    ST_ID identifier = st_enter_id(typename);
    return ty_build_unresolved_ptr(identifier);
  }
}

PARAM_LIST merge_param_lists(PARAM_LIST list1, PARAM_LIST list2)
{
    message("merge param lists");
}

typedef_list make_new_type_list(TYPE t)
{
    message("make new type list");
    // Allocate a new typedef_item_p value.
    typedef_item_p value = (typedef_item_p) malloc(sizeof(TYPEDEF_ITEM));
    
    // Explicitly set the next pointer to NULL and copy the types from item
    value->next = NULL;
    value->new_def = NULL; //not sure what this should be
    value->old_type = t;
    
    return value;
}

typedef_list append_to_type_list(typedef_list list, TYPE t)
{
    message("append to type list");
    typedef_list current = list;

    while (current->next) 
    {
        current = current->next;
    }

    current->next = t;
}

void vardec(stid_list list, TYPE t)
{
    message("vardec");

    //create ST data record
    ST_DR dr;

    //while list is not null
    while (list)
    {
        dr = stdr_alloc();
        dr->tag = GDECL;
        dr->u.decl.type = t;
        st_install(list->enrollment_papers, dr);
        message("added type: ");
        //ty_print_type(t); //currently gives "illegal typetag"
        list = list->next;
    }
}
