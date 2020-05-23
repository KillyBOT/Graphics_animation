#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ml6.h"
#include "matrix.h"
#include "draw.h"

#define BUFFER_SIZE 512

struct matrix* stlConvert(struct matrix* m, char* fileName){
	FILE* f = fopen(fileName,"r");

	char* buffer;
	float* binaryVertices;
	char* sBuffer;
	int vCount = 0;
	double vertices[3];
	int bufferPlace = 0;

	buffer = malloc(BUFFER_SIZE);

	fgets(buffer, BUFFER_SIZE, f);
	fseek(f, 0, SEEK_SET);

	if(!strncmp(buffer,"solid",5)){

		while(fgets(buffer,BUFFER_SIZE,f) != NULL){
			buffer[strlen(buffer)-1] = '\0';
			sBuffer = strdup(buffer);

			while(sBuffer[0] == ' ' || sBuffer[0] == '\t'){
				strsep(&sBuffer," \t");
			}
			//printf("%s\n", sBuffer);

			if(sBuffer[0] == 'v'){

				// if(vCount == 0){
				// 	fprintf(f,"polygon\n");
				// }

				//strsep(&buffer," ");
				//printf("%s\n", buffer);

				//vCount++;
				sscanf(sBuffer,"vertex %lf %lf %lf", vertices, vertices+1, vertices+2);

				//printf("%lf %lf %lf\n", vertices[0], vertices[1], vertices[2]);

				//fprintf(f,"%lf %lf %lf ", vertices[0], vertices[1], vertices[2]);
				add_point(m, vertices[0], vertices[1], vertices[2]);

				// if(vCount >= 3){
				// 	fprintf(f,"\n");
				// 	vCount = 0;
				// }
			}
			//printf("\n");
		}

	} else {

		unsigned int tNum;
		short int data;
		binaryVertices = malloc(sizeof(float) * 3);

		fread(buffer, sizeof(char), 80, f);
		//printf("Mesh name: %s\n", buffer);

		fread(&tNum, sizeof(unsigned int), 1, f);
		//printf("Number of polygons: %d\n", tNum);

		for(unsigned int n = 0; n < tNum; n++){
			fread(binaryVertices, sizeof(float), 3, f);

			for(int o = 0; o < 3; o++){
				fread(binaryVertices, sizeof(float),3,f);

				add_point(m, binaryVertices[0], binaryVertices[1], binaryVertices[2]);
			}

			fread(&data, sizeof(short int), 1, f);

		}

	}

	free(buffer);
	fclose(f);

	return m;
}