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
 
 stid_item_p new_stid_list(ST_ID root)
 {
    // Allocate a new stid_item_p value.
    stid_item_p value = (stid_item_p) malloc(sizeof(STID_ITEM));
    
    // Explicitly set the next pointer to NULL and set the enrollment_papers.
    value.next = NULL;
    value.enrollment_papers = root;
    
    return value;
 }
 
 stid_list append_to_stid_list(stid_list base, ST_ID new_id)
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
    
    // Return the list again.
    return base;
    
 }
