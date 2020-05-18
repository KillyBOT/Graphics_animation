#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kdTree.h"
#include "gmath.h"

struct kdTree* kdCreate(){
	struct kdTree* kd = malloc(sizeof(struct kdTree));

	kd->size = 0;
	kd->root = NULL;

	return kd;
}
void kdFree(struct kdTree* kd){
	if(kd->root != NULL) kdFree_helper(kd->root);

	free(kd);
}
void kdFree_helper(struct kdNode* k){
	if(k->left != NULL) kdFree_helper(k->left);
	if(k->right != NULL) kdFree_helper(k->right);

	free(k);
}
struct kdNode* kdNodeCreate(double* vertex, double* normal){
	struct kdNode* k = malloc(sizeof(struct kdNode));

	k->vertex[0] = vertex[0];
	k->vertex[1] = vertex[1];
	k->vertex[2] = vertex[2];

	k->normal[0] = normal[0];
	k->normal[1] = normal[1];
	k->normal[2] = normal[2];

	k->c.red = 0;
	k->c.green = 0;
	k->c.blue = 0;

	k->left = NULL;
	k->right = NULL;

	return k;
}

struct kdTree* kdInsert(struct kdTree* kd, double* vertex, double* normal){
	kd->root = kdInsert_helper(kd->root, vertex, normal, 0);
	return kd;
}
struct kdNode* kdInsert_helper(struct kdNode* k, double* vertex, double* normal, int depth){
	if(k == NULL) return kdNodeCreate(vertex, normal);
	if(k->vertex[0] == vertex[0] && k->vertex[1] == vertex[1] && k->vertex[2] == vertex[2]){
		k->normal[0] += normal[0];
		k->normal[1] += normal[1];
		k->normal[2] += normal[2];

		return k;
	}

	if(k->vertex[depth] > vertex[depth]){
		k->left = kdInsert_helper(k->left, vertex, normal, (depth + 1) % MAXDEPTH);
	} else {
		k->right = kdInsert_helper(k->right, vertex, normal, (depth+1) % MAXDEPTH);
	}

	return k;
}
struct kdNode* kdGetNode(struct kdTree* kd, double* vertex){
	return kdGetNode_helper(kd->root, vertex, 0);
}
struct kdNode* kdGetNode_helper(struct kdNode* k, double* vertex, int depth){
	if(k == NULL){
		printf("You shouldn't be seeing this!\n");
		return NULL;
	}
	if(k->vertex[0] == vertex[0] && k->vertex[1] == vertex[1] && k->vertex[2] == vertex[2]){
		return k;
	}

	if(k->vertex[depth] > vertex[depth]){
		return kdGetNode_helper(k->left, vertex, (depth + 1) % MAXDEPTH);
	} else {
		return kdGetNode_helper(k->right, vertex, (depth + 1) % MAXDEPTH);
	}
}
double* kdGetNormal(struct kdTree* kd, double* vertex){
	return kdGetNode(kd, vertex)->normal;
}
color kdGetColor(struct kdTree* kd, double* vertex){
	return kdGetNode(kd, vertex)->c;
}

struct kdTree* kdNormalize(struct kdTree* kd, double *view, double light[2][3], color ambient, struct constants* reflect){
	kd->root = kdNormalize_helper(kd->root, view, light, ambient, reflect);
	return kd;
}
struct kdNode* kdNormalize_helper(struct kdNode* k, double *view, double light[2][3], color ambient, struct constants* reflect){
	if(k->left != NULL) k->left = kdNormalize_helper(k->left, view, light, ambient, reflect);
	if(k->right != NULL) k->right = kdNormalize_helper(k->right, view, light, ambient, reflect);

	normalize(k->normal);
	k->c = get_lighting(k->normal, view, ambient, light, reflect);

	return k;
}

void kdPrint(struct kdTree* kd){
	kdPrint_helper(kd->root, 0);
}
void kdPrint_helper(struct kdNode* k, int depth){
	if(k->left != NULL) kdPrint_helper(k->left, depth+1);
	if(k->right != NULL) kdPrint_helper(k->right, depth+1);

	for(int x = 0; x < depth; x++) printf("\t");
	printf("Vertex: %f %f %f\n",k->vertex[0],k->vertex[1],k->vertex[2]);

	for(int x = 0; x < depth; x++) printf("\t");
	printf("Normal: %f %f %f\n",k->normal[0],k->normal[1],k->normal[2]);

	for(int x = 0; x < depth; x++) printf("\t");
	printf("Color: %d %d %d\n", k->c.red, k->c.green, k->c.blue);


}