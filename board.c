#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "board.h"

//Removes letter in position p from string w
//Returns removed letter
char remove_letter(char w[], int p){
  int i = p;
  char removed_letter = w[p];
  while(w[i + 1]){
    w[i] = w[i + 1];
    i++;
  }
  w[i] = '\0';
  return removed_letter;
}

//Generates a random letter from the six possibilities for a cube's face
char rand_face(char *letters){
  srand(time(NULL));
  return letters[rand() % 6];
}

//Takes a game board and returns a two-dimensional character array of the faces of the cubes on the board
char **board_to_chars(struct cube ***the_board, int size){
  char **ans = (char **)malloc(5 * sizeof(char *));
  int i, j;
  for(i = 0; i < 5; i++)
    ans[i] = (char *)malloc(5 * sizeof(char));
  for(i = 0; i < size; i++)
    for(j = 0; j < size; j++)
      ans[i][j] = *get_face_board(i, j, the_board);
  return ans;
}
	/*
int main(){
  //struct cube ***five_board = (struct cube ***)initialize5();
  struct cube ***four_board = (struct cube ***)initialize4();
  print5(five_board);
  printf("\n");
  print4(four_board);
  printf("The letter at (1,2) is: %s and its checked status is: %d\n", get_face_board(1, 2, four_board), is_checked_board(1,2, four_board));
  check_board(1, 2, four_board);
  printf("The letter at (1,2) now is: %s and its checked status is: %d\n", get_face_board(1, 2, four_board), is_checked_board(1,2, four_board));
  uncheck_all(four_board, 4);
  printf("The letter at (1,2) now is: %s and its checked status is: %d\n", get_face_board(1, 2, four_board), is_checked_board(1,2, four_board));
  set_face(four_board[0][0], 'q');
  set_face(four_board[0][1], 'i');
  set_face(four_board[1][1], 't');
  print4(four_board);
  printf("quit is a word: %d\n", on_board("quit", four_board, 4));
  set_face(four_board[1][1], 'q');
  print4(four_board);
  printf("Ewe is a word: %d\n", on_board("ewe", four_board, 4));
  set_face(four_board[0][0], 'e');
  set_face(four_board[1][1], 'w');
  set_face(four_board[2][2], 'm');
  set_face(four_board[2][3], 'l');
  set_face(four_board[3][3], 'a');
  set_face(four_board[1][2], 'm');
  set_face(four_board[0][2], 'e');

  print4(four_board);
  printf("Emma is a word: %i\n", on_board("emma", four_board, 4));
  printf("Emmm is a word: %i\n", on_board("emmm", four_board, 4));
  char ** the_board = board_to_chars(four_board, 4);
  int i, j;
 for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
    printf("%c", the_board[i][j]);
  return 0;
  }*/


//Returns the letter on the face of the cube at position (x,y) on board the_board
char *get_face_board(int x, int y, struct cube ***the_board){
  char *ans = (char *)malloc(sizeof(char *));
  ans[0] = get_face(the_board[x][y]);
  return ans;
}

//Returns whether the cube at position (x,y) on board the_board has been used in checking for a word
int is_checked_board(int x, int y, struct cube ***the_board){
  return is_checked(the_board[x][y]);
}

//Marks all cubes on board the_board as unused in check for a word
void uncheck_all(struct cube ***the_board, int size){
  int i, j;
  for(i = 0; i < size; i++)
    for(j = 0; j < size; j++)
      uncheck(the_board[i][j]);
}

//Checks whether a word is long enough to be counted
int check_length(char *word, int size){
  return strlen(word) >= (size - 1);
}

//Marks the cube at position (x,y) on board the_board as having been used
void check_board(int x, int y, struct cube ***the_board){
  check(the_board[x][y]);
}

//Marks the cube at position (x,y) on board the_board as not having been used
void uncheck_board(int x, int y, struct cube ***the_board){
  uncheck(the_board[x][y]);
}

//Returns whether word is on board the_board
//Looks through the board and calls the helper function on any occurence of the first letter of word
//When on_board_helper returns true or when the entire board has been searched unchecks the first letter and returns
int on_board(char *word, struct cube ***the_board, int size){
  int i = 0, j = 0;
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      //printf("Checking %i,%i\n", i, j);
      if(is_letter(i, j, word, size, the_board)){
	check_board(i, j, the_board);
	if(on_board_helper(behead(word), i, j, the_board, size)){
	  uncheck_board(i, j, the_board);
	  return 1;
	}
      }
      uncheck_board(i, j, the_board);
    }
  }
  return 0;
}

//Copies word, removes the first letter from the copy, and returns the copy
char *behead(char *word){
  //printf("Beheading\n");
  char *ord = malloc(strlen(word));
  strncpy(ord, word, strlen(word));
  //printf("Beheading: %s\n", ord);
  if(remove_letter(ord, 0) == 'q')
    remove_letter(ord, 0);
  //printf("Beheaded: %s\n", ord);
  return ord;
}

//Checks whether location indicated by coordinates (x,y) and contains the first letter of string word
int is_letter(int x, int y, char *word, int size, struct cube ***the_board){
  if(x < 0 || y < 0 || x >= size || y >= size)
    return 0;
  //printf("Comparing %s & %s\n", word, get_face_board(x, y, the_board));
  return !strncmp(word, get_face_board(x, y, the_board), 1);
}

//Recursive function helper to on_board
//Returns true if word is the empty string (meaning all letters of the word were found in order)
//Otherwise:
//Looks at each letter around the given location, calling itself again if the first letter of the remaining word is found
//Checks a cube that follows in the word, unchecking it if it produces a false lead 
int on_board_helper(char *word, int i, int j, struct cube ***the_board, int size){

  //printf("Here she goes...%s\n", word);
  int x, y;
  if(strlen(word) == 0)
    return 1;

  //Upper Left
  x = i - 1;
  y = j - 1;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 1...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }

  //Left
  x = i;
  y = j - 1;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 2...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }
  
  //Lower Left
  x = i + 1;
  y = j - 1;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 3...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }

  //Above
  x = i - 1;
  y = j;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 4...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }

  //Below
  x = i + 1;
  y = j;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 5...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }

  //Upper Right
  x = i - 1;
  y = j + 1;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 6...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }

  //Right
  x = i;
  y = j + 1;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 7...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }

  //Lower right
  x = i + 1;
  y = j + 1;
  if(is_letter(x, y, word, size, the_board) && !is_checked_board(x, y, the_board)){
    printf("Case 8...\n");
    check_board(x, y, the_board);
    if(on_board_helper(behead(word), x, y, the_board, size)){
      uncheck_board(x, y, the_board);
      return 1;
    }
    else
      uncheck_board(x, y, the_board);
  }
  return 0;
}
