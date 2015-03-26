#include "eval.h"
#include "tree.h"
#include <stdlib.h>

int evaluate(struct st *node) {
	if (node->type == 'C') {
		return node->value;
	} else if (node->type == 'V') {
		return get_val(node->var);
	} else if (node->type == 'P') {
		return evaluate(node->left);
	} else if (node->type == 'N') {
		return -evaluate(node->left);
	} else {
		int v1 = evaluate(node->left);
		int v2 = evaluate(node->right);
		switch(node->type) {
			case '+': return v1 + v2;
			case '-': return v1 - v2;
			case '*': return v1 * v2;
			case '/': return v1 / v2;
			case '%': return v1 % v2;
		}
	}
}

static int val_tab[26];

int get_val(int v)
{
    return val_tab[v - 'A'];
}

void set_val(int v, struct st *node)
{
    val_tab[v - 'A'] = evaluate(node);
}