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
	char* sBuffer;
	int vCount = 0;
	double vertices[3];
	int bufferPlace = 0;

	buffer = malloc(BUFFER_SIZE);

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

	free(buffer);
	fclose(f);

	return m;
}