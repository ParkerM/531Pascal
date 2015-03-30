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


// ***** IDENTIFIER-RELATED CONSTRUCTIONS ***** //
/* A structure that holds items to be stored in an ST_ID list.
 * 
 * Each item points to the next in the list and stores the value of the ST_ID it encases.
 */
typedef struct stid_item
{
    struct stid_item* next;
    ST_ID enrollment_papers;
} STID_ITEM, *stid_item_p;

typedef stid_item_p stid_list;

/* Function to create a new list of ST_IDs using the given ST_ID as its root. */
stid_list new_stid_list(ST_ID root);

/* Function to append a new ST_ID to an existing list of such items. */
void append_stid_to_list(stid_list base, ST_ID new_id);



// ***** TYPE-RELATED CONSTRUCTIONS ***** //

typedef struct typedef_item
{
    struct typedef_item* next;
    ST_ID new_def;
    TYPE old_type;
} TYPEDEF_ITEM, *typedef_item_p, *typedef_list;

/* Function to create a typedef_list node from the given ST_ID and TYPE. */
typedef_item_p make_typedef_node(ST_ID id, TYPE t);

/* Function to append a new typedef_item to an existing list of such items. */
void append_typedef_to_list(typedef_list base, typedef_item_p newItem);

/* Function to process a typedef_list */
void process_typedefs(typedef_list list);

/* Function to merge two param lists if separated by semicolon */
PARAM_LIST merge_param_lists(PARAM_LIST list1, PARAM_LIST list2);

/* Function to create a new type list when an ordinal index type is encountered */
typedef_list make_new_type_list(TYPE t);

/* Function to append a TYPE to an existing typedef_list */
typedef_list append_to_type_list(typedef_list list, TYPE t);
#endif
