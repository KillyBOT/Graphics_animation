#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASHTABLE_SIZE 65536

#include "ml6.h"
#include "symtab.h"

struct htElement{
	double vertex[3];
	double normal[3];
	int num;
	int key;
	color c;
};

struct hashTable {
	struct htElement** ht;
	int size;
	int maxSize;
	//unsigned int (*hash) (double*);
};

// struct htElement** createHT();
// void freeHT(struct htElement** ht);

// unsigned int getKey(double * vertex);

// void addNormal(struct htElement** ht, double* vertex, double* normal);
// double * getNormal(struct htElement** ht, double * vertex);
// color getColor(struct htElement** ht, double* vertex);
// double * getVertexKey(struct htElement** ht, int key);
// void printHT(struct htElement** ht);
// void htNormalize(struct htElement** ht, double *view, double light[2][3], color ambient,
//   double *areflect, double *dreflect, double *sreflect);

struct hashTable* createHT(int size);
void freeHT(struct hashTable* h);

unsigned int getKey(struct hashTable* h, double * vertex);

struct hashTable* addNormal(struct hashTable* h, double* vertex, double* normal);
double * getNormal(struct hashTable* h, double * vertex);
color getColor(struct hashTable* h, double* vertex);
double * getVertexKey(struct hashTable* h, int key);
void printHT(struct hashTable* h);
void htNormalize(struct hashTable* h, double *view, double light[2][3], color ambient,
  struct constants* reflect);
#endif