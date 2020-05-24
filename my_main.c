/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"

/*======== void first_pass() ==========
  Inputs:
  Returns:

  Checks the op array for any animation commands
  (frames, basename, vary)

  Should set num_frames and basename if the frames
  or basename commands are present

  If vary is found, but frames is not, the entire
  program should exit.

  If frames is found, but basename is not, set name
  to some default value, and print out a message
  with the name being used.
  ====================*/
void first_pass() {
  //These variables are defined at the bottom of symtab.h
  extern int num_frames;
  extern char name[128];

  int varyFound, framesFound, basenameFound;
  varyFound = 0;
  framesFound = 0;
  basenameFound = 0;

  num_frames = 1;

  for(int i=0;i<lastop;i++){
    //printf("%d: ", i);
    switch(op[i].opcode){
      case FRAMES:

        num_frames = (int)op[i].op.frames.num_frames;

        framesFound = 1;

        break;

      case BASENAME:

        strcpy(name, op[i].op.basename.p->name);

        basenameFound = 1;

        break;

      case VARY:

        varyFound = 1;

        break;
    }
  }

  if(varyFound && !framesFound){
    printf("Frames not found! Exiting...\n");
    exit(1);
  }

  if(framesFound && !basenameFound){

    strcpy(name,"fyjshk");

    printf("Basename not found! Defaulting to %s\n", name);
  }

}

/*======== struct vary_node ** second_pass() ==========
  Inputs:
  Returns: An array of vary_node linked lists

  In order to set the knobs for animation, we need to keep
  a seaprate value for each knob for each frame. We can do
  this by using an array of linked lists. Each array index
  will correspond to a frame (eg. knobs[0] would be the first
  frame, knobs[2] would be the 3rd frame and so on).

  Each index should contain a linked list of vary_nodes, each
  node contains a knob name, a value, and a pointer to the
  next node.

  Go through the opcode array, and when you find vary, go
  from knobs[0] to knobs[frames-1] and add (or modify) the
  vary_node corresponding to the given knob with the
  appropirate value.
  ====================*/
struct vary_node ** second_pass() {

  extern int num_frames;
  extern char name[128];

  struct vary_node *curr = NULL;
  struct vary_node **knobs;
  knobs = (struct vary_node **)calloc(num_frames, sizeof(struct vary_node *));

  double valChange;
  double currentVal;

  for(int i = 0; i < lastop; i++){
    switch(op[i].opcode){
      case VARY:

        valChange = (op[i].op.vary.end_val - op[i].op.vary.start_val)/(op[i].op.vary.end_frame - op[i].op.vary.start_frame);
        currentVal = op[i].op.vary.start_val;

        add_symbol(op[i].op.vary.p->name, SYM_VALUE, 0);

        for(int f = (int)op[i].op.vary.start_frame; f <= (int)op[i].op.vary.end_frame; f++){

          curr = knobs[f];

          if(curr == NULL){
            knobs[f] = malloc(sizeof(struct vary_node));

            strcpy(knobs[f]->name,op[i].op.vary.p->name);
            knobs[f]->value = currentVal;
            knobs[f]->next = NULL;
          }
          else {
            while(curr->next != NULL){
              curr = curr->next;
            }

            curr->next = malloc(sizeof(struct vary_node));

            strcpy(curr->next->name, op[i].op.vary.p->name);
            curr->next->value = currentVal;
            curr->next->next = NULL;
          }

          currentVal += valChange;
        }

        break;
    }
  }

  return knobs;
}

void my_main() {

  struct vary_node ** knobs;
  struct vary_node * vn;
  struct vary_node* curr;
  first_pass();
  printf("First pass complete\n");
  knobs = second_pass();
  printf("Second pass complete\n");
  char frame_name[128];
  int f;

  int i;
  struct matrix *tmp;
  struct stack *systems;
  screen t;
  zbuffer zb;
  double step_3d = 100;
  double theta, xval, yval, zval;

  //Lighting values here for easy access
  color ambient;
  ambient.red = 50;
  ambient.green = 50;
  ambient.blue = 50;

  double light[2][3];
  light[LOCATION][0] = 0.5;
  light[LOCATION][1] = 0.75;
  light[LOCATION][2] = 1;

  light[COLOR][RED] = 255;
  light[COLOR][GREEN] = 255;
  light[COLOR][BLUE] = 255;

  double view[3];
  view[0] = 0;
  view[1] = 0;
  view[2] = 1;

  //default reflective constants if none are set in script file
  struct constants white;
  white.r[AMBIENT_R] = 0.1;
  white.g[AMBIENT_R] = 0.1;
  white.b[AMBIENT_R] = 0.1;

  white.r[DIFFUSE_R] = 0.5;
  white.g[DIFFUSE_R] = 0.5;
  white.b[DIFFUSE_R] = 0.5;

  white.r[SPECULAR_R] = 0.5;
  white.g[SPECULAR_R] = 0.5;
  white.b[SPECULAR_R] = 0.5;

  //constants are a pointer in symtab, using one here for consistency
  struct constants *reflect;
  reflect = &white;

  color g;
  g.red = 255;
  g.green = 255;
  g.blue = 255;

  for(f = 0; f < num_frames; f++){

    systems = new_stack();
    tmp = new_matrix(4, 1024);
    clear_screen( t );
    clear_zbuffer(zb);

    curr = knobs[f];

    while(curr != NULL){
      lookup_symbol(curr->name)->s.value = curr->value;
      curr = curr->next;
    }

    for (i=0;i<lastop;i++) {

      //printf("%d: ",i);
      switch (op[i].opcode)
        {
        case SPHERE:
          // printf("Sphere: %6.2f %6.2f %6.2f r=%6.2f",
          //        op[i].op.sphere.d[0],op[i].op.sphere.d[1],
          //        op[i].op.sphere.d[2],
          //        op[i].op.sphere.r);
          if (op[i].op.sphere.constants != NULL) {
            //printf("\tconstants: %s",op[i].op.sphere.constants->name);
            reflect = lookup_symbol(op[i].op.sphere.constants->name)->s.c;
          }
          if (op[i].op.sphere.cs != NULL) {
            //printf("\tcs: %s",op[i].op.sphere.cs->name);
          }
          add_sphere(tmp, op[i].op.sphere.d[0],
                     op[i].op.sphere.d[1],
                     op[i].op.sphere.d[2],
                     op[i].op.sphere.r, step_3d);
          matrix_mult( peek(systems), tmp );
          draw_polygons(tmp, t, zb, view, light, ambient,
                        reflect);
          tmp->lastcol = 0;
          reflect = &white;
          break;
        case TORUS:
          // printf("Torus: %6.2f %6.2f %6.2f r0=%6.2f r1=%6.2f",
          //        op[i].op.torus.d[0],op[i].op.torus.d[1],
          //        op[i].op.torus.d[2],
          //        op[i].op.torus.r0,op[i].op.torus.r1);
          if (op[i].op.torus.constants != NULL) {
            //printf("\tconstants: %s",op[i].op.torus.constants->name);
            reflect = lookup_symbol(op[i].op.sphere.constants->name)->s.c;
          }
          if (op[i].op.torus.cs != NULL) {
            //printf("\tcs: %s",op[i].op.torus.cs->name);
          }
          add_torus(tmp,
                    op[i].op.torus.d[0],
                    op[i].op.torus.d[1],
                    op[i].op.torus.d[2],
                    op[i].op.torus.r0,op[i].op.torus.r1, step_3d);
          matrix_mult( peek(systems), tmp );
          draw_polygons(tmp, t, zb, view, light, ambient,
                        reflect);
          tmp->lastcol = 0;
          reflect = &white;
          break;
        case BOX:
          // printf("Box: d0: %6.2f %6.2f %6.2f d1: %6.2f %6.2f %6.2f",
          //        op[i].op.box.d0[0],op[i].op.box.d0[1],
          //        op[i].op.box.d0[2],
          //        op[i].op.box.d1[0],op[i].op.box.d1[1],
          //        op[i].op.box.d1[2]);
          if (op[i].op.box.constants != NULL) {
            //printf("\tconstants: %s",op[i].op.box.constants->name);
            reflect = lookup_symbol(op[i].op.sphere.constants->name)->s.c;
          }
          if (op[i].op.box.cs != NULL) {
            //printf("\tcs: %s",op[i].op.box.cs->name);
          }
          add_box(tmp,
                  op[i].op.box.d0[0],op[i].op.box.d0[1],
                  op[i].op.box.d0[2],
                  op[i].op.box.d1[0],op[i].op.box.d1[1],
                  op[i].op.box.d1[2]);
          matrix_mult( peek(systems), tmp );
          draw_polygons(tmp, t, zb, view, light, ambient,
                        reflect);
          tmp->lastcol = 0;
          reflect = &white;
          break;
        case MESH:

          if(op[i].op.mesh.constants != NULL){
            reflect = lookup_symbol(op[i].op.mesh.constants->name)->s.c;
          }

          stlConvert(tmp, op[i].op.mesh.name);
          matrix_mult(peek(systems),tmp);

          draw_polygons(tmp, t, zb, view, light, ambient, reflect);
          tmp->lastcol = 0;
          reflect = &white;
          break;

        case LINE:
          // printf("Line: from: %6.2f %6.2f %6.2f to: %6.2f %6.2f %6.2f",
          //        op[i].op.line.p0[0],op[i].op.line.p0[1],
          //        op[i].op.line.p0[2],
          //        op[i].op.line.p1[0],op[i].op.line.p1[1],
          //        op[i].op.line.p1[2]);
          if (op[i].op.line.constants != NULL) {
            //printf("\n\tConstants: %s",op[i].op.line.constants->name);
          }
          if (op[i].op.line.cs0 != NULL) {
            //printf("\n\tCS0: %s",op[i].op.line.cs0->name);
          }
          if (op[i].op.line.cs1 != NULL) {
            //printf("\n\tCS1: %s",op[i].op.line.cs1->name);
          }
          add_edge(tmp,
                   op[i].op.line.p0[0],op[i].op.line.p0[1],
                   op[i].op.line.p0[2],
                   op[i].op.line.p1[0],op[i].op.line.p1[1],
                   op[i].op.line.p1[2]);
          matrix_mult( peek(systems), tmp );
          draw_lines(tmp, t, zb, g);
          tmp->lastcol = 0;
          break;
        case MOVE:
          xval = op[i].op.move.d[0];
          yval = op[i].op.move.d[1];
          zval = op[i].op.move.d[2];
          //printf("Move: %6.2f %6.2f %6.2f", xval, yval, zval);
          if (op[i].op.move.p != NULL) {
            //printf("\tknob: %s",op[i].op.move.p->name);
            xval *= lookup_symbol(op[i].op.move.p->name)->s.value;
            yval *= lookup_symbol(op[i].op.move.p->name)->s.value;
            zval *= lookup_symbol(op[i].op.move.p->name)->s.value;
          }
          tmp = make_translate( xval, yval, zval );
          matrix_mult(peek(systems), tmp);
          copy_matrix(tmp, peek(systems));
          tmp->lastcol = 0;
          break;
        case SCALE:
          xval = op[i].op.scale.d[0];
          yval = op[i].op.scale.d[1];
          zval = op[i].op.scale.d[2];
          //printf("Scale: %6.2f %6.2f %6.2f", xval, yval, zval);
          if (op[i].op.scale.p != NULL) {
            //printf("\tknob: %s",op[i].op.scale.p->name);
            
            xval *= lookup_symbol(op[i].op.scale.p->name)->s.value;
            yval *= lookup_symbol(op[i].op.scale.p->name)->s.value;
            zval *= lookup_symbol(op[i].op.scale.p->name)->s.value;
          }
          tmp = make_scale( xval, yval, zval );
          matrix_mult(peek(systems), tmp);
          copy_matrix(tmp, peek(systems));
          tmp->lastcol = 0;
          break;
        case ROTATE:
          theta =  op[i].op.rotate.degrees * (M_PI / 180);
          if (op[i].op.rotate.p != NULL) {
            //printf("\tknob: %s",op[i].op.rotate.p->name);
            theta *= lookup_symbol(op[i].op.rotate.p->name)->s.value;
          }
          //printf("Rotate: axis: %6.2f degrees: %6.2f", op[i].op.rotate.axis, theta);

          if (op[i].op.rotate.axis == 0 )
            tmp = make_rotX( theta );
          else if (op[i].op.rotate.axis == 1 )
            tmp = make_rotY( theta );
          else
            tmp = make_rotZ( theta );

          matrix_mult(peek(systems), tmp);
          copy_matrix(tmp, peek(systems));
          tmp->lastcol = 0;
          break;
        case LIGHT:

          for(int x = 0; x < 3; x++){
            light[LOCATION][x] = op[i].op.light.p->s.l->l[x];
            light[COLOR][x] = op[i].op.light.p->s.l->c[x];
          }

          break;
        case AMBIENT:

          white.r[AMBIENT_R] = op[i].op.ambient.c[0];
          white.g[AMBIENT_R] = op[i].op.ambient.c[1];
          white.b[AMBIENT_R] = op[i].op.ambient.c[2];

          break;
        case PUSH:
          //printf("Push");
          push(systems);
          break;
        case POP:
          //printf("Pop");
          pop(systems);
          break;
        case SAVE:
          if(num_frames <= 1){
            //printf("Save: %s",op[i].op.save.p->name);
            save_extension(t, op[i].op.save.p->name);
            break;
          }
        case DISPLAY:
          if(num_frames <= 1){
            //printf("Display");
            display(t);
            break;
          }
        }
      //printf("\n");
    } //end operation loop

    if(num_frames > 1){
      char fileName[256];
      if(f < 10) sprintf(fileName, "%s_000%d.png", name, f);
      else if(f < 100) sprintf(fileName, "%s_00%d.png", name, f);
      else if(f < 1000) sprintf(fileName, "%s_0%d.png", name, f);
      else sprintf(fileName, "%s_%d.png", name, f);
      save_extension(t, fileName);
    }

    free_stack( systems );
    free_matrix( tmp );

  }

  if(num_frames > 1){
    int status;

    int n = fork();


    if(n == 0){ //Child

      double delayTime;
      char fileArg[256];
      char delayArg[256];
      char finalFileArg[256];

      sprintf(fileArg, "%s_*",name);
      sprintf(finalFileArg, "%s.gif",name);

      delayTime = 1.7;
      sprintf(delayArg, "%.1f",delayTime);

      execlp("convert", "convert", "-delay", delayArg, fileArg, finalFileArg, NULL);
    } else {
      printf("Converting to gif...\n");
      waitpid(n, &status, 0);
    }

    n = fork();

    if(n == 0){
      char removeArg[256];

      for(f = 0; f < num_frames; f++){
        if(f < 10) sprintf(removeArg, "%s_000%d.png", name, f);
        else if(f < 100) sprintf(removeArg, "%s_00%d.png", name, f);
        else if(f < 1000) sprintf(removeArg, "%s_0%d.png", name, f);
        else sprintf(removeArg, "%s_%d.png", name, f);
        remove(removeArg);
      }

      exit(0);
    } else {
      printf("Cleaning up...\n");
      waitpid(n, &status, 0);
    }
  }

}
