


#ifndef __BTREE_H
#define __BTREE_H

typedef struct _btree_node {
	
	unsigned long value;
	int height;
	int weight;
	
	struct _btree_node * left;
	struct _btree_node * right;
	
} btree_t;

void insert_btree(btree_t ** root, unsigned long value);
char contains_btree(btree_t * root, unsigned long value);

void free_btree(btree_t * root);

int height_btree(btree_t * root);
int weight_btree(btree_t * root);

#endif