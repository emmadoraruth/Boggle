#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>


struct link * wordlist;

//Returns true if word is already contained in word_list (is a duplicate); false otherwise
int duplicate(char **word_list, char *word){
  int i = 0;
  while(strlen(word_list[i])){
    if(!strcmp(word_list[i], word))
      return 1;
    i++;
  }
  return 0;
}

//Converts a word to all lowercase letters
void uncapitalize(char *word){
  int i;
  for(i = 0; i < strlen(word); i++){
    if(word[i] > 64 && word[i] < 91)
      word[i] += 32;
  }
}

/*======== void catch_alarm () ==========
 Inputs:  int sig 
 Returns: 
 
 
 12/27/09 00:40:03
 leekuhn
 ====================*/
void catch_alarm (int sig){
  if ( sig == SIGALRM ) {
    clear();
    refresh();
    endwin();
    printf("Ending one child: %d\n", getpid());
    _exit(1);
  }
}

/*======== int main() ==========
 Inputs:   
 Returns: 
 
 Establishes an ncurses window which:
 - Displays a boggle board for 3 minutes
 - Allows a player to enter words
 - Checks to see if the word is on the board
 - Displays all words entered
 
 12/26/09 20:07:14
 leekuhn
 ====================*/
void interface(char board[5][5], char **words, int size, int id){

  int i = 0, j = 0, x = 2, y = 0, winx, winy, word = 0, fd;
  char * test = (char *)malloc(16);
  char * temp = (char *)malloc(13 * sizeof(char));
  sprintf(temp, "%d",id);
  strcat(temp, "WriteTest");
  fd = open(temp, O_RDWR | O_CREAT, 0664);

  // Initial ncurses setup
  (void) initscr();      // initialize the curses library
  keypad(stdscr, TRUE);  // enable keyboard mapping
  nonl();                // tell curses not to do NL->CR/NL on output
  //cbreak();              // take input chars one at a time, no wait for \n
  echo();                // don't echo input 
  scrollok(stdscr, TRUE);
	
  // Set an alarm
  signal (SIGALRM, (sig_t)catch_alarm);
  alarm(180);
	
  move(0,0);
  if(size == 5)
    printw("---------------------\n");
  else
    printw("-----------------\n");
  refresh();
  move(1,0);
  for(i = 0; i < size; i++){
    printw("|");
    for(j = 0; j < size; j++){
      move(i * 2 + 1, j * 4 + 1);
      if(board[i][j] == 'q')
	printw("q u|");
      else
	printw(" %c |", board[i][j]);
      refresh();
    }
    if(size == 5)
      printw("\n---------------------\n");
    else
      printw("\n-----------------\n");
    refresh();
  }
  x = i * 2 + 1;
	
  // Establish the prompt
  move(x, 0);
  refresh();
  printw("Enter a word: ");
  refresh();
  x ++;
	
  while(1){
    move(i * 2 + 1, 16);           // Go to prompt
    clrtoeol();               // Clear entered text
    getstr(test);             // Take entered text
		
    //Check the window size.
    getmaxyx(stdscr, winx, winy); 
		
    // Check if a new column is needed
    if(x >= winx){
      x = i * 2 + 2;
      y = y + 17;
    }
		
    // Move to next place in list and show word
    move(x,y); 
    x++;
    printw("%s", test);
    uncapitalize(test);
    if(!duplicate(words, test)){
      // Add test to word list if it is not already there
      write(fd, test, strlen(test));
      write(fd, "\n", 1);
      strcpy(words[word], test);
      word++;
    }
    refresh();
		
  }
  endwin();
	
}

char** copy_array(int id){
  //initialize variables
  int dic, x, y, count, num;
  num = 60;
  char** dicArray;
  char* temp;
  temp = (char*)malloc(15*num);
  dicArray = (char**)malloc(num*sizeof(char*));
  for(x=0; x<num; x++){
    dicArray[x] = (char*)malloc(256);
  }
  char * temp2 = (char *)malloc(13 * sizeof(char));
  sprintf(temp2, "%d",id);
  strcat(temp2, "WriteTest");

  //reads dictionary file into a temporary string
  dic = open(temp2, O_RDONLY, 0777);
  y = read(dic, temp, (15*num));
  //printf("%s", temp);
  if(y == -1){
    printf("Copy array Error\n");
    return NULL;
  }
  count = 0;
  //printf("%s", strsep(&temp, " "));
  //separates temp string and puts words into char** array
  while(temp){
    dicArray[count] = strsep(&temp, "\n");
    count++;
  }
  /*
    for(y=0; y<count; y++){
    printf("%s ", dicArray[y]);
    }
  */
  remove(temp2);
  return dicArray;
}

/*
int main(){
  int q;
  int p;
  int x;
  char temp[16];
  char board[5][5];
  char ** words;
  int p2 = getpid();
  words = (char **)malloc(60 * sizeof(char *));
  for(q = 0; q < 60; q++)
    words[q] = (char *)malloc(25 * sizeof(char));

  strcpy(board[0], "qjkjr");
  strcpy(board[1], "hkjhf");
  strcpy(board[2], "bmvxc");
  strcpy(board[3], "werty");
  strcpy(board[4], "ertyu");
	

  //vfork allows memory to be shared between forked processes
  p = fork();
  
  if(p == 0)
    interface(board, words, 5);
  else{
    wait(NULL);
    q = 0;
    words = copy_array();
    printf("In child %s\n", words[0]);
    while(strlen(words[q])){
      sleep(1);
      printf("%s\n", words[q]);
      q++;
    }
  }

	
  /*
    if ( p != 0 ) { 
    printf("**************************\n");
    q = 0;
    while(strlen(words[q])){
    sleep(1);
    printf("%s\n", words[q]);
    q++;
    }
    return 0;
    }
	
    else {  
    p = fork();
    //Parent runs the game. This ensures that the original "words" will be edited
    if(p != 0){
    //printf("Test1 %d\n", getpid());
    interface(board, words, 5);
    exit(0);
    }
    //Child sleeps until game should be ended, then sends a SIGALRM to that process
    else{
    //printf("%d\n", p2 + 1);
    sleep(15);
    kill(getppid(), SIGALRM);
    }
    }
}
*/
