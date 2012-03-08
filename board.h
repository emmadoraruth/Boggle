#include "cube.h"

char remove_letter(char w[], int p);
char rand_face(char * letters);
char *get_face_board(int x, int y, struct cube ***the_board);
int is_checked_board(int x, int y, struct cube ***the_board);
void uncheck_all(struct cube ***the_board, int size);
void check_board(int x, int y, struct cube ***the_board);
void uncheck_board(int x, int y, struct cube ***the_board);
int check_length(char *word, int size);
struct cube ***initialize4();
struct cube ***initialize5();
void print4(struct cube ***board);
void print5(struct cube ***board);
int on_board_helper(char *word, int i, int j, struct cube ***the_board, int size);
int is_letter(int x, int y, char *word, int size, struct cube ***the_board);
int on_board(char *word, struct cube ***the_board, int size);
char *behead(char *word);
char **board_to_chars(struct cube ***the_board, int size);
int check_length(char *word, int size);
