#include <stdio.h>
#include <stdlib.h>

#include "cube.h"

//Return the letter on the face of the cube
char get_face(struct cube *c){
  return c -> face;
}

//Return whether the cube has already been used in a word
//Used for verifying whether a word is on the board
int is_checked(struct cube *c){
  return c -> checked;
}

//Set the letter on the face of the cube
void set_face(struct cube *c, char letter){
  c -> face = letter;
}

//Mark the cube as having been used
void check(struct cube *c){
  c -> checked = 1;
}

//Unmark the cube to begin a fresh check for a word
void uncheck(struct cube *c){
  c -> checked = 0;
}

//Initialize an unchecked cube
struct cube *init_cube(char my_face){
  struct cube *the_cube = (struct cube *)malloc(sizeof(struct cube));
  set_face(the_cube, my_face);
  uncheck(the_cube);
  return the_cube;
}
