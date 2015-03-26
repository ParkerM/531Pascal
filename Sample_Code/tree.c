#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

struct st **tptr;
int tptr_size = 0;

struct st *new_node(int type, struct st *left, struct st *right) {
	struct st *node = malloc(sizeof(struct st));
	node->type = type;
	node->left = left;
	node->right = right;

	return node;
}

struct st *new_term_node(int type, int value) {
	struct st *node = malloc(sizeof(struct st));
	node->type = type;
	node->value = value;

	return node;
}

struct st *new_var_node(int var, int value) {
	struct st *node = malloc(sizeof(struct st));
	node->type = 'V';
	node->var = var;
	node->value = value;

	return node;
}

void store_tree(struct st *node) {

	int i;

	struct st **temp = tptr;

	//increase size by one
	tptr = (struct st *) malloc((tptr_size + 1) * sizeof(struct st));

	for (i = 0; i < tptr_size; i++) {
		tptr[i] = temp[i];
	}
	tptr[tptr_size] = node;
	tptr_size++;

	free(temp);
}

struct st *get_tree(int index) {
	if (index <= 0) {
		fflush(stdout);
		fprintf(stderr, "\nError: index %d out of range\n", index);
		exit(1); //failure
	} else if (index > tptr_size) {
		fflush(stdout);
		fprintf(stderr, "\nError: index %d out of range\n", index);
		exit(1); //failure
	}
	return tptr[index - 1];
}