/*
 * TREE.H
 *
 * This header file declares the various structures and methods used to create, manipulate, and parse the syntax
 * tree(s) created during the parsing of the Pascal file.
 *
 * Author(s): Michael Brunson
 *            John Mauney
 *            Myndert Papenhuyzen
 *
 *      Date: March 29, 2015
 *
 *   Purpose: CSCE 531 (Compiler Construction) Project
 */

#ifndef __TREE_H
#define __TREE_H

#include <stdlib.h>

#include "types.h"   // Imports type-related methods and structures.
#include "symtab.h"  // Import symbol table-related methods and structures.

/* A structure that holds items to be stored in an ST_ID list.
 * 
 * Each item points to the next in the list and stores the value of the ST_ID it encases.
 */
typedef struct stid_item
{
    stid_item* next;
         ST_ID enroll_papers;
}
STID_ITEM, *stid_item_p

typedef stid_item_p stid_list;

/* Function to create a new list of ST_IDs using the given ST_ID as its root. */
stid_list new_stid_list(ST_ID root);

/* Function to append a new ST_ID to an existing list of such items. */
stid_list append_to_stid_list(stid_list base, ST_ID new_id);
#endif
