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
 #include "types.h"
 #include "symtab.h"
 
 stid_list new_stid_list(ST_ID root)
 {
    // Allocate a new stid_item_p value.
    stid_item_p value = (stid_item_p) malloc(sizeof(STID_ITEM));
    
    // Explicitly set the next pointer to NULL and set the enrollment_papers.
    value.next = NULL;
    value.enrollment_papers = root;
    
    return value;
 }
 
 void append_stid_to_list(stid_list base, ST_ID new_id)
 {
    // Allocate a new stid_item_p item.
    stid_item_p newItem = (stid_item_p) malloc(sizeof(STID_ITEM));
    
    // Explicitly set the next pointer to NULL and set the enrollment_papers.
    newItem.next = NULL;
    newItem.enrollment_papers = new_id;
    
    // Now, loop through the list until the last item is found.
    stid_item_p currentItem = base;
    
    while (currentItem.next)
    {
        currentItem = currentItem.next;
    }
    
    // Append the newItem to the end of the list.
    currentItem.next = newItem;
    
 }
 
 typedef_item_p make_typedef_node(ST_ID id, TYPE t)
 {
    typedef_item_p new_typedef = (typedef_item_p) malloc(sizeof(TYPEDEF_ITEM));
    
    new_typedef.next = NULL;
    new_typedef.new_def = id;
    new_typedef.old_type = t;
 }
 
void append_typedef_to_list(typedef_list base, typedef_item_p newItem)
{
    typedef_item_p current = base;
    
    while (current.next)
    {
        current = current.next;
    }
    
    current.next = newItem;
}
