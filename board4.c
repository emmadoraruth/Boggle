#define A "dlxeri"
#define B "nhiumq"
#define C "eehngw"
#define D "skffap"
#define E "wtooat"
#define F "bbooaj"
#define G "ohpcas"
#define H "idystt"
#define I "yvdrle"
#define J "siteos"
#define K "sineeu"
#define L "rtlyte"
#define M "eengaa"
#define N "nnhlzr"
#define O "tvrhew"
#define P "toimuc"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "board.h"

//Takes a char (the_cube) chosen randomly from a string representing all cubes
//Matches that char to the appropriate cube
//Returns the string corresponding to the faces on that cube
char *letter_to_cube4(char the_cube){
	if(the_cube == 'A')
		return A;
	else if(the_cube == 'B')
		return B;
	else if(the_cube == 'C')
		return C;
	else if(the_cube == 'D')
		return D;
	else if(the_cube == 'E')
		return E;
	else if(the_cube == 'F')
		return F;
	else if(the_cube == 'G')
		return G;
	else if(the_cube == 'H')
		return H;
	else if(the_cube == 'I')
		return I;
	else if(the_cube == 'J')
		return J;
	else if(the_cube == 'K')
		return K;
	else if(the_cube == 'L')
		return L;
	else if(the_cube == 'M')
		return M;
	else if(the_cube == 'N')
		return N;
	else if(the_cube == 'O')
		return O;
	else
		return P;
}

//Initializes and returns a 4x4 board
struct cube ***initialize4(){

  struct cube ***board = (struct cube ***)malloc(4 * sizeof(struct cube **));
  char cubes[] = "ABCDEFGHIJKLMNOP";
  char face;
  //struct cube * the_cube;
  int p, i, j, ran;
  srand(time(NULL));

  //Malloc the array of pointers
  board = (struct cube ***)malloc(4 * sizeof(struct cube **));
  for (p = 0; p < 4; p++){
    board[p] = (struct cube **)malloc(sizeof(struct cube *));
    board[p][0] = (struct cube *)malloc(sizeof(struct cube));
  }
  
  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      //Get a random number up to the size of cubes (number of remaining cubes)
      ran = rand() % strlen(cubes);
      //Remove a letter from cubes
      face = remove_letter(cubes, ran);
      //printf("The Cube: %c\n", face);
      //Initialize the cube corresponding to that letter in the current position
      board[i][j] = init_cube(rand_face((char *)letter_to_cube4(face)));
      //board[i][j] = the_cube;
      //printf("Letter at (%d,%d) on 4X4 is %c\n", i, j, get_face(board[i][j]));
    }
  }
  return board;
}

//Prints 4x4 board
void print4(struct cube ***board){
  int i, j;
  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++)
      printf("%c|", get_face(board[i][j]));
    printf("\n");
  }
}
