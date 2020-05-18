#include <stdio.h>
#include <stdlib.h>

#include "hashTable.h"
#include "gmath.h"

//This is using a new hashTable structure

struct hashTable* createHT(int size){
	struct hashTable* h = malloc(sizeof(struct hashTable));
	h->size = 0;
	h->maxSize = size;

	h->ht = malloc(sizeof(struct htElement) * h->maxSize);

	for(int n = 0; n < h->maxSize; n++){
		h->ht[n] = NULL;
	}

	return h;
}

void freeHT(struct hashTable* h){
	for(int x = 0; x < h->maxSize; x++){
		if(h->ht[x] != NULL) free(h->ht[x]);
	}
	free(h->ht);
	free(h);
}

unsigned int getKey(struct hashTable* h, double * vertex){
	unsigned int f = ((unsigned int)vertex[0] + (unsigned int)vertex[1] * 11 - (unsigned int)vertex[2] * 17) & 0x7fffffff;
	return ((f >> 22) ^ (f >> 12) ^ f) % h->maxSize;
}

struct hashTable* addNormal(struct hashTable* h, double* vertex, double* normal){
	//printf("Test\n");

	struct hashTable* hNew;

	if(h->size >= h->maxSize){
		//printf("Expanding from %d to %d\n",h->size,h->size*2);
		hNew = createHT(h->maxSize*2);

		for(int n = 0; n < h->maxSize; n++){
			addNormal(hNew, h->ht[n]->vertex,h->ht[n]->normal);
		}
		
		freeHT(h);
		addNormal(hNew, vertex, normal);
		return hNew;
	}

	unsigned int index = getKey(h, vertex);

	while(h->ht[index] != NULL && (h->ht[index]->vertex[0] != vertex[0] || h->ht[index]->vertex[1] != vertex[1] || h->ht[index]->vertex[2] != vertex[2])){
		index++;
		index %= h->maxSize;
	}

	if(h->ht[index] == NULL) {
		h->ht[index] = malloc(sizeof(struct htElement));
		h->ht[index]->vertex[0] = vertex[0];
		h->ht[index]->vertex[1] = vertex[1];
		h->ht[index]->vertex[2] = vertex[2];
		h->ht[index]->normal[0] = normal[0];
		h->ht[index]->normal[1] = normal[1];
		h->ht[index]->normal[2] = normal[2];
		h->ht[index]->num = 1;
		h->size++;
	} else {
		h->ht[index]->normal[0] += normal[0];
		h->ht[index]->normal[1] += normal[1];
		h->ht[index]->normal[2] += normal[2];
		h->ht[index]->num++;
	}

	return h;
}

double* getNormal(struct hashTable* h, double* vertex){
	// double vNew[3];
	// vNew[0] = (int)(vertex[0]*1000)/1000.0;
	// vNew[1] = (int)(vertex[1]*1000)/1000.0;
	// vNew[2] = (int)(vertex[2]*1000)/1000.0;

	unsigned int index = getKey(h, vertex);

	while(h->ht[index]->vertex[0] != vertex[0] || h->ht[index]->vertex[1] != vertex[1] || h->ht[index]->vertex[2] != vertex[2]){
		index++;
		index %= h->maxSize;
	}
	//printf("%d %d %d\n", ht[index]->c.red, ht[index]->c.green, ht[index]->c.blue);
	return h->ht[index]->normal;
	//return c;
}

color getColor(struct hashTable* h, double* vertex){
	// double vNew[3];
	// vNew[0] = (int)(vertex[0]*1000)/1000.0;
	// vNew[1] = (int)(vertex[1]*1000)/1000.0;
	// vNew[2] = (int)(vertex[2]*1000)/1000.0;

	unsigned int index = getKey(h, vertex);

	while(h->ht[index]->vertex[0] != vertex[0] || h->ht[index]->vertex[1] != vertex[1] || h->ht[index]->vertex[2] != vertex[2]){
		index++;
		index %= h->maxSize;
	}
	//printf("%d %d %d\n", ht[index]->c.red, ht[index]->c.green, ht[index]->c.blue);
	return h->ht[index]->c;
	//return c;
}

void htNormalize(struct hashTable* h, double *view, double light[2][3], color ambient, struct constants* reflect){
	for(int x = 0; x < h->maxSize; x++){
		if(h->ht[x] != NULL){
			//h->ht[x]->normal[0] /= h->ht[x]->num;
			//h->ht[x]->normal[1] /= h->ht[x]->num;
			//h->ht[x]->normal[2] /= h->ht[x]->num;
			normalize(h->ht[x]->normal);
			//printf("%f\n",h->ht[x]->normal[0]*h->ht[x]->normal[0] + h->ht[x]->normal[1]*h->ht[x]->normal[1] + h->ht[x]->normal[2]*h->ht[x]->normal[2]);
			
			h->ht[x]->c = get_lighting(h->ht[x]->normal, view, ambient, light, reflect);

		}
	}
}

void printHT(struct hashTable* h){
	for(int x = 0; x < h->maxSize; x++){
		if(h->ht[x] != NULL){
			printf("%f %f %f\n\t%f %f %f\n\tIntensity:%d %d %d\n", h->ht[x]->vertex[0], h->ht[x]->vertex[1], h->ht[x]->vertex[2], h->ht[x]->normal[0], h->ht[x]->normal[1], h->ht[x]->normal[2],h->ht[x]->c.red, h->ht[x]->c.green, h->ht[x]->c.blue);
		}
	}
	printf("Total size: %d\n\n", h->size);
}