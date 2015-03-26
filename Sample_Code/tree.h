#ifndef TREE_H
#define TREE_H

struct st *new_node(int type, struct st *left, struct st *right);
struct st *new_term_node(int type, int value);
struct st *new_var_node(int var, int value);

struct st 
{
	int type;
	int value;
	int var;
	struct st *left;
	struct st *right;
};

#endif
