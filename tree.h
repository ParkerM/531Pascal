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

#include "expr.h"
#include "encode.h"
#include "message.h"
#include "types.h"   // Imports type-related methods and structures.
#include "symtab.h"  // Import symbol table-related methods and structures.

// ***** NUMERICAL CONSTANT-RELATED CONSTRUCTIONS ***** //
/* A structure meant to hold a numerical constant of both the integral and real varieties.
 */
typedef struct number_constant
{
  TYPETAG type;
  union
  {
    long integer;
    double real;
  } v;
} num_const, *num_const_p;

/* Creates a number constant structure from an integer. */
num_const_p allocate_number_const_int(long i);

/* Creates a number constant structure from a double (real). */
num_const_p allocate_number_const_real(double d);

/* Signes the number constant, based in the input for i (1 or -1 only, please!) */
num_const_p sign_constant(int i, num_const_p number);



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
stid_list append_stid_to_list(stid_list base, ST_ID new_id);



// ***** TYPE-RELATED CONSTRUCTIONS ***** //
typedef struct typedef_item
{
    ST_ID new_def;
    TYPE old_type;
} TYPEDEF_ITEM, *typedef_item_p;

/* Function to create a typedef_item node from the given ST_ID and TYPE. */
typedef_item_p make_typedef_node(ST_ID id, TYPE t);

/* Function to install a new typedef_item into the symbol table. */
void install_typedef(typedef_item_p item);

/* Function to process unresolved pointer types */
void process_unresolved_types();

/* Processes the string to get the basic type. */
TYPE get_basic_type(char*);

/* Creates a subrange type from the given constants. As of PROJECT 1, the only num_consts
   accepted are integer constants. */
TYPE create_subrange(EXPR low, EXPR high);

BOOLEAN isSimpleType(TYPE t);
BOOLEAN isDataType(TYPE t);
BOOLEAN isOrdinalType(TYPETAG t);

/* Function to create a new param list when a formal parameter is encountered */
PARAM_LIST make_new_param_list(TYPE t);

/* Function ot create a new param list from a list of IDs. */
PARAM_LIST make_new_param_list_from_ids(stid_item_p list);

/* Function to append a PARAM to an existing param_list */
PARAM_LIST append_to_param_list(PARAM_LIST list, PARAM_LIST p);

/* Function to merge two param lists if separated by semicolon */
PARAM_LIST merge_param_lists(PARAM_LIST list1, PARAM_LIST list2);

/* Function to convert an ST_ID list to a PARAM_LIST of the supplied type */
PARAM_LIST id_list_to_param_list(stid_list idList, TYPE listType, BOOLEAN isVar);

/* Function to create a new type list when an ordinal index type is encountered */
TYPE_LIST make_new_type_list(TYPE t);

/* Function to append a TYPE to an existing typedef_list */
TYPE_LIST append_to_type_list(TYPE_LIST list, TYPE t);

/* Associate id_list with type and add to ST DR */
void vardec(stid_list list, TYPE t);

/* Associate id_list with type as a set of parameters*/
void paramdec(stid_list list, TYPE t);

stid_list merge_stid_list(stid_list list1, stid_list list2);



/* -----===== Control Structure-related Constructions =====----- */

typedef struct
{
    char *conditional_label;
    char *after_label;
} control_labels;

typedef enum { FOR_TO, FOR_DOWNTO } FOR_DIRECTION;
#endif
