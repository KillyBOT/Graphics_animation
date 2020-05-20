#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ml6.h"
#include "matrix.h"
#include "draw.h"

struct matrix* stlConvert(struct matrix* m, char* fileName){
	FILE* sF = fopen(fileName,"r");

	char buffer[512];
	char* stringBuffer = malloc(512);
	int vCount = 0;
	double vertices[3];
	int bufferPlace = 0;

	while(fgets(buffer,sizeof(buffer),sF) != NULL){
		buffer[strlen(buffer)-1] = '\0';
		strcpy(stringBuffer,buffer);

		while(stringBuffer[0] == ' ' || stringBuffer[0] == '\t'){
			strsep(&stringBuffer," \t");
		}
		//printf("%s\n", stringBuffer);

		if(stringBuffer[0] == 'v'){

			// if(vCount == 0){
			// 	fprintf(f,"polygon\n");
			// }

			//strsep(&buffer," ");
			//printf("%s\n", buffer);

			//vCount++;
			sscanf(stringBuffer,"vertex %lf %lf %lf", vertices, vertices+1, vertices+2);

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


	free(stringBuffer);
	fclose(sF);
	//fclose(f);

	return m;
}