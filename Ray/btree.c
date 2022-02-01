#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/*
typedef struct _btree_node {
	
	unsigned long value;
	struct _btree_node * left;
	struct _btree_node * right;
	
} btree_t;
*/




int height_btree(btree_t * root) {
	if (root == NULL) return 0;
	return root->height;
}

int weight_btree(btree_t * root) {
	if (root == NULL) return 0;
	return root->weight;
}




void set_height(btree_t * root) {
	if(root == NULL) return;
	int l = height_btree(root->left);
	int r = height_btree(root->right);
	root->height = 1 + (l >= r ? l : r);
}

void set_weight(btree_t * root) {
	if (root == NULL) return;
	root->weight = 1 + weight_btree(root->left) + weight_btree(root->right);
}

int balance_btree(btree_t * root) {
	if (root == NULL) return 0;
	int l = height_btree(root->left);
	int r = height_btree(root->right);
	return l - r;
}


void insert_btree(btree_t ** root, unsigned long value) {
	if (*root == NULL) {
		btree_t * rtn = (btree_t *) malloc(sizeof(btree_t));
		rtn->value = value;
		rtn->height = 1;
		rtn->weight = 1;
		rtn->left = NULL;
		rtn->right = NULL;
		*root = rtn;
		return;
	}
	
	btree_t * A = (*root);
	
	if (A->value == value) {
		return;
	} else if (A->value > value) {
		insert_btree(&(A->left), value);
	} else {
		insert_btree(&(A->right), value);
	}
	
	set_height(A);
	set_weight(A);
	
	int balance = balance_btree(A);
	
	if (balance > 1) {
		btree_t * B = A->left;
		btree_t * E = B->right;

		int balance2 = balance_btree(B);

		if (balance2 > 0) {
			/*
				 A		|   B
				/ \		|  / \
			   B   C 	| D   A
			  / \		|    / \
			 D   E		|   E   C
			*/

			*root = B;
			B->right = A;
			A->left = E;
			
			set_height(A);
			set_height(B);
			
			set_weight(A);
			set_weight(B);
			
		} else if (balance2 < 0){
			/*
				 A		|       A	|         E
				/ \		|      / \	|       /   \
			   B   C 	|     E	  C	|      B     A
			  / \		|    / \	|     / \   / \
			 D   E		|   B   K	|    D   J K   C
				/ \		|  / \		|
			   J   K	| D   J		|
			*/

			btree_t * J = E->left;
			btree_t * K = E->right;

			*root = E;
			E->left = B;
			E->right = A;
			B->right = J;
			A->left = K;
			
			set_height(A);
			set_height(B);
			set_height(E);
			
			set_weight(A);
			set_weight(B);
			set_weight(E);
			
		}
	} else if (balance < -1) {
		btree_t * C = A->right;
		btree_t * F = C->left;

		int balance2 = balance_btree(C);

		if (balance2 > 0) {
			/*
			   A		|   A		|        F
			  / \		|  / \		|      /   \
			 B   C		| B   F		|     A     C
				/ \		|    / \	|    / \   / \
			   F   G	|   L   C	|   B   L M   G
			  / \		|      / \	|
			 L   M		|     M	  G	|
			*/

			btree_t * L = F->left;
			btree_t * M = F->right;

			*root = F;
			F->left = A;
			F->right = C;
			A->right = L;
			C->left = M;
			
			set_height(A);
			set_height(C);
			set_height(F);
			
			set_weight(A);
			set_weight(C);
			set_weight(F);
			
		} else if (balance2 < 0) {
			/*
			   A		|      C
			  / \		|     / \
			 B   C		|    A   G
			    / \		|   / \
			   F   G 	|  B   F
			*/

			*root = C;
			C->left = A;
			A->right = F;
			
			set_height(A);
			set_height(C);
			
			set_weight(A);
			set_weight(C);
		}
	}
}

char contains_btree(btree_t * root, unsigned long value) {
	
	while (root != NULL) {
		
		if (root->value == value) {
			return 1;
		} else if (root->value > value) {
			root = root->left;
		} else {
			root= root->right;
		}
	}
	
	return 0;
	
}

/*
int count_btree(btree_t * root) {
	if (root == NULL) return 0;
	return 1 + count_btree(root->left) + count_btree(root->right);
}
int to_array_btree(btree_t * root, unsigned long * data) {
	if (root == NULL) return 0;

	int cLeft = count_btree(root->left);
	int cRight = count_btree(root->right);

	to_array_btree(root->left, data);
	data[cLeft] = root->value
	to_array_btree(root->right, data + 1 + cLeft);

	return cLeft + 1 + cRight;

}
*/

void free_btree(btree_t * root) {
	if (root == NULL) {
		return;
	} else {
		free_btree(root->left);
		free_btree(root->right);
		free(root);
	}
}

