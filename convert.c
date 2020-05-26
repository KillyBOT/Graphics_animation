#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ml6.h"
#include "matrix.h"
#include "draw.h"

#define BUFFER_SIZE 512

struct matrix* convert(struct matrix* m, char* fileName){
	FILE* f = fopen(fileName,"r");

	char* fileType;
	char* buffer;
	float* binaryVertices;
	char* sBuffer;
	int vCount = 0;
	double vertices[4];
	double textureCoords[4];
	double vNormals[4];
	int bufferPlace = 0;

	buffer = malloc(BUFFER_SIZE);

	fileType = strrchr(fileName, '.');
	fileType++;

	if(!strcmp(fileType,"stl")){ //Stl file
		fgets(buffer, BUFFER_SIZE, f);
		fseek(f, 0, SEEK_SET);

		if(!strncmp(buffer,"solid",5)){ //ASCII STL file

			while(fgets(buffer,BUFFER_SIZE,f) != NULL){
				buffer[strlen(buffer)-1]='\0';
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

		} else { //Binary STL file

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
	}

	else if(!strcmp(fileType,"obj")){ //Obj file

		struct matrix* vList = new_matrix(4, 4096);
		add_point(vList, 0, 0, 0);

		char* lineType;
		char* lengthCheck;
		int spaceNum;

		while(fgets(buffer,BUFFER_SIZE,f) != NULL){
			buffer[strlen(buffer)-1] = '\0';
			sBuffer = strdup(buffer);
			//printf("Raw data: %s\n", sBuffer);

			lineType = strsep(&sBuffer," ");
			if(sBuffer != NULL){

				while(sBuffer != NULL && (sBuffer[0] == ' ' || sBuffer[0] == '\t')) strsep(&sBuffer," \t");

				if(sBuffer != NULL){
					//printf("Line type: {%s}\n", lineType);
					//printf("%s\n", sBuffer);

					if(!strcmp(lineType,"v")){
						sscanf(sBuffer, "%lf %lf %lf", vertices, vertices+1, vertices+2);
						add_point(vList,vertices[0],vertices[1],vertices[2]);
					}

					else if(!strcmp(lineType,"f")){

						spaceNum = 0;

						lengthCheck = strdup(sBuffer);
						while(*lengthCheck != '\0'){
							if(*lengthCheck == ' ') spaceNum++;
							lengthCheck++;
						}

						if(spaceNum == 2){ //Triangle
							sscanf(sBuffer, "%lf/%lf/%lf %lf/%lf/%lf %lf/%lf/%lf",
								vertices, textureCoords, vNormals,
								vertices+1, textureCoords+1, vNormals+1,
								vertices+2, textureCoords+2, vNormals+2);

							add_point(m, vList->m[0][(int)vertices[0]], vList->m[1][(int)vertices[0]], vList->m[2][(int)vertices[0]]);
							add_point(m, vList->m[0][(int)vertices[1]], vList->m[1][(int)vertices[1]], vList->m[2][(int)vertices[1]]);
							add_point(m, vList->m[0][(int)vertices[2]], vList->m[1][(int)vertices[2]], vList->m[2][(int)vertices[2]]);
						} else { //Rectangle
							sscanf(sBuffer, "%lf/%lf/%lf %lf/%lf/%lf %lf/%lf/%lf %lf/%lf/%lf",
								vertices, textureCoords, vNormals,
								vertices+1, textureCoords+1, vNormals+1,
								vertices+2, textureCoords+2, vNormals+2,
								vertices+3, textureCoords+3, vNormals+3);

							add_point(m, vList->m[0][(int)vertices[0]], vList->m[1][(int)vertices[0]], vList->m[2][(int)vertices[0]]);
							add_point(m, vList->m[0][(int)vertices[1]], vList->m[1][(int)vertices[1]], vList->m[2][(int)vertices[1]]);
							add_point(m, vList->m[0][(int)vertices[2]], vList->m[1][(int)vertices[2]], vList->m[2][(int)vertices[2]]);

							add_point(m, vList->m[0][(int)vertices[0]], vList->m[1][(int)vertices[0]], vList->m[2][(int)vertices[0]]);
							add_point(m, vList->m[0][(int)vertices[2]], vList->m[1][(int)vertices[2]], vList->m[2][(int)vertices[2]]);
							add_point(m, vList->m[0][(int)vertices[3]], vList->m[1][(int)vertices[3]], vList->m[2][(int)vertices[3]]);
						}

						
					}

				}
				
			}

			
		}

		free_matrix(vList);
	}

	

	free(buffer);
	fclose(f);

	return m;
}