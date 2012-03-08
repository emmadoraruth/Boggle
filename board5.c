#define A "aeaeee"
#define B "cwncts"
#define C "eeguam"
#define D "hirpry"
#define E "aafars"
#define F "iieclt"
#define G "ticlep"
#define H "pyfisr"
#define I "itetii"
#define J "toouwn"
#define K "nuesss"
#define L "tdnhoh"
#define M "nennad"
#define N "jqzbkx"
#define O "ecipst"
#define P "rnlhdo"
#define Q "ddroln"
#define R "raisfy"
#define S "aeemee"
#define T "gmnaen"
#define U "rowgrv"
#define V "toootu"
#define W "rifsaa"
#define X "tmteot"
#define Y "rlhhdo"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "board.h"

//Takes a char (the_cube) chosen randomly from a string representing all cubes
//Matches that char to the appropriate cube
//Returns the string corresponding to the faces on that cube
char *letter_to_cube5(char the_cube){
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
	else if(the_cube == 'P')
		return P;
	else if(the_cube == 'Q')
		return Q;
	else if(the_cube == 'R')
		return R;
	else if(the_cube == 'S')
		return S;
	else if(the_cube == 'T')
		return T;
	else if(the_cube == 'U')
		return U;
	else if(the_cube == 'V')
		return V;
	else if(the_cube == 'W')
		return W;
	else if(the_cube == 'X')
		return X;
	else
		return Y;
}

//Initializes and returns a 5x5 board
struct cube ***initialize5(){
	
	struct cube ***board = (struct cube ***)malloc(5 * sizeof(struct cube **));
	char cubes[] = "ABCDEFGHIJKLMNOPQRSTUVWXY";
	char face;
	//struct cube * the_cube;
	int p, i, j, ran;
	srand(time(NULL));
	
	//Malloc the array of pointers
	board = (struct cube ***)malloc(5 * sizeof(struct cube **));
	for (p = 0; p < 5; p++){
		board[p] = (struct cube **)malloc(sizeof(struct cube *));
		board[p][0] = (struct cube *)malloc(sizeof(struct cube));
	}
	
	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++){
			//Get a random number up to the size of cubes (number of remaining cubes)
			ran = rand() % strlen(cubes);
			//Remove a letter from cubes
			face = remove_letter(cubes, ran);
			//printf("The Cube: %c\n", face);
			//Initialize the cube corresponding to that letter in the current position
			board[i][j] = init_cube(rand_face((char *)letter_to_cube5(face)));
			//board[i][j] = the_cube;
			//printf("Letter at (%d,%d) on 5X5 is %c\n", i, j, get_face(board[i][j]));
		}
	}
	return board;
}

//Prints 5x5 board
void print5(struct cube ***board){
	int i, j;
	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++)
			printf("%c|", get_face(board[i][j]));
		printf("\n");
	}
}