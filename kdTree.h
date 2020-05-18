#ifndef KDTREE_H
#define KDTREE_H

#include "ml6.h"
#include "symtab.h"

#define MAXDEPTH 3

struct kdNode{
	double vertex[3];
	double normal[3];
	color c;
	struct kdNode* left;
	struct kdNode* right;
};

struct kdTree {
	struct kdNode* root;
	int size;
};

struct kdTree* kdCreate();
void kdFree(struct kdTree* kd);
void kdFree_helper(struct kdNode* k);
struct kdNode* kdNodeCreate(double* vertex, double* normal);

struct kdTree* kdInsert(struct kdTree* kd, double* vertex, double* normal);
struct kdNode* kdInsert_helper(struct kdNode* k, double* vertex, double* normal, int depth);
struct kdNode* kdGetNode(struct kdTree* kd, double* vertex);
struct kdNode* kdGetNode_helper(struct kdNode* k, double* vertex, int depth);
struct kdTree* kdNormalize(struct kdTree* kd, double *view, double light[2][3], color ambient, struct constants* reflect);
struct kdNode* kdNormalize_helper(struct kdNode* k, double *view, double light[2][3], color ambient, struct constants* reflect);
double* kdGetNormal(struct kdTree* kd, double* vertex);
color kdGetColor(struct kdTree* kd, double* vertex);

void kdPrint(struct kdTree* kd);
void kdPrint_helper(struct kdNode* k, int depth);

#endif