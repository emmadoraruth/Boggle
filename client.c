#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "message.h"
#include "board.h"
#include "game.h"


int main(int argc, char **argv){
	
  int socket_id, id_num, i = 0, x, udp_id, e, w, p, q;
  char username[18], buffer[4];
  struct message *m = (struct message *)malloc(sizeof(struct message));
  struct sockaddr_in sock, udp;
  char ** words = (char **)malloc(60 * sizeof(char *));
  for(q = 0; q < 60; q++)
    words[q] = (char *)malloc(17 * sizeof(char));
	
  //Have user choose a username
  //__fpurge(stdin);
  printf("Enter a username of 16 characters or fewer: ");
  fgets(username, sizeof(username), stdin);
  username[16] = '\n';
  *(strchr(username, '\n')) = '\0';
  printf("\nUsername: %s\n", username);
  strncpy(m -> username, username, 16);
	
  umask(0000);	
	
  //TCP
  //make the server socket for reliable IPv4 traffic 
  socket_id = socket( AF_INET, SOCK_STREAM, 0);
	
	
  //set up the server socket struct
  //Use IPv4 
  sock.sin_family = AF_INET;
	
  //Client will connect to address in argv[1], need to translate that IP address to binary
  inet_aton( argv[1], &(sock.sin_addr) );
    
  //set the port to listen on, htons converts the port number to network format
  sock.sin_port = htons(24611);
	
  //connect to the server
  //connect(socket_id, (struct sockaddr *)&sock, sizeof(sock));
	
  // printf("TCP set up on client\n");
	
	
  //UDP
  //make the server socket for reliable IPv4 traffic 
  udp_id = socket( AF_INET, SOCK_DGRAM, 0);
	
	
  //set up the server socket struct
  //Use IPv4 
  udp.sin_family = AF_INET;
	
  //Client will connect to address in argv[1], need to translate that IP address to binary
  inet_aton( argv[1], &(udp.sin_addr) );
    
  //set the port to listen on, htons converts the port number to network format
  udp.sin_port = htons(24612);
	
  //connect to the server
  w = connect(udp_id, (struct sockaddr *)&udp, sizeof(udp));
	
  //printf("UDP set up on client: %s\n", argv[1]);
	
  ///must do the first message read/write outside of the
  //loop due to the select statement
  m->type = CONNECT;
	
  e = (int)sendto(udp_id, m, sizeof(struct message), 0, NULL, 0);
  //printf("Sent, Sendto returns: %d, connect returns: %d\n", e, w);
  usleep(200);
  connect(socket_id, (struct sockaddr *)&sock, sizeof(sock));
  printf("Connected to server\n");	
  //get id assigned by server
  read(socket_id, m, sizeof(struct message));
  if (m -> type == QUIT) {
    close(socket_id);
    close(udp_id);
    free(m);
    printf("Server closed");
    exit(0);
  }
  id_num = m -> id_num;
  printf("My new id is: %d\n", m->id_num);
	
  //Do client stuff continuously
  while(1){
    while(1){
			
      //Request games info
      m -> type = REQUEST_INFO;
      write(udp_id, m, sizeof(struct message));
      read(socket_id, m, sizeof(struct message));
      if (m -> type == QUIT) {
	close(socket_id);
	close(udp_id);
	free(m);
	printf("Server closed");
	exit(0);
      }
      i = 0;
      for(q = 0; q < 8; q++)
	if(m -> games[1][q] != -1 && m -> games[3][q] >= 0 && m -> games[4][q] >= 0){
	  printf("Game %d:\n\tBoard Size: %dx%d\n\tPlayers (so far): %d\n\tTime until start: %d hours, %d minutes, %d seconds\n", q, m -> games[1][q], m -> games[1][q], m -> games[0][q], m -> games[2][q], m -> games[3][q], m -> games[4][q]);
	  i++;
	}
			
      //User picks a game, or makes a new game
      //fpurge(stdin);
      //strcpy(buffer, "");
      if(i)
	printf("Enter number of game you would like to join (games with 10 players are full)\n");
      if(i < 8)
	printf("Enter 44 to start a new game with board size 4x4 or 55 to start a new game with board size 5x5\n");
      printf("Enter 'r' or 'R' to refresh\nEnter 'q' or 'Q' to quit\n");
      fgets(buffer, sizeof(buffer), stdin);
      printf("Input is: %s\n", buffer);
      if (buffer [0] == 'q' || buffer[0] == 'Q') {
	m -> type = QUIT;
	write(udp_id, m, sizeof(struct message));
	close(socket_id);
	close(udp_id);
	free(m);
	exit(0);
      }
      if (buffer [0] == 'r' || buffer[0] == 'R')
	break;
      //*(strchr(buffer, '\n')) = 0;
      buffer[2] = '\0';
      x = (int)strtol(buffer, (char **)NULL, 10);
      printf("Input converted to int is: %d\n", x);
      //If user has attempted to enter a game that does not exist, to start a new game when all games are full, or enter a full game, keep prompting for a legal game number
      while(x < 0 || ((x >= i) && (x != 44 && x!= 55)) || (((x == 44) || (x == 55)) && i > 8) || (x < i && m -> games[0][x] >= 10)){
	//fpurge(stdin);
	if(x < i && m -> games[0][x] >= 10)
	  printf("Illegal choice! You have selected a game that is already full. Please try again.\n");
	else      
	  printf("Illegal choice! You have selected an option that does not exist. Please try again.\n");
	fgets(buffer, sizeof(buffer), stdin);
	if (buffer [0] == 'q' || buffer[0] == 'Q') {
	  m -> type = QUIT;
	  write(udp_id, m, sizeof(struct message));
	  close(socket_id);
	  close(udp_id);
	  free(m);
	  exit(0);
	}
	if (buffer [0] == 'r' || buffer[0] == 'R')
	  break;
	buffer [2] = '\0';
	x = (int)strtol(buffer, (char **)NULL, 10);
      }
      if (buffer [0] == 'r' || buffer[0] == 'R')
	break;
      //If the user has selected to start a new game, prompt for the time to start, and fill m accordingly
      if(x == 44 || x == 55){
	m -> type = REQUEST_NEW_GAME;
	m -> games[0][0] = 1;
	if(x == 44)
	  m -> games[1][0] = 4;
	else
	  m -> games[1][0] = 5;
	//fpurge(stdin);
	printf("Please enter the amount of time in which you would like to begin your game.\nHours: ");
	fgets(buffer, sizeof(buffer), stdin);
	*(strchr(buffer, '\n')) = '\0';
	x = (int)strtol(buffer, NULL, 10);
	m -> games[2][0] = x;
	//fpurge(stdin);
	printf("Minutes: ");
	fgets(buffer, sizeof(buffer), stdin);
	*(strchr(buffer, '\n')) = '\0';
	x = (int)strtol(buffer, NULL, 10);
	m -> games[3][0] = x;
	//fpurge(stdin);
	printf("Seconds: ");
	fgets(buffer, sizeof(buffer), stdin);
	*(strchr(buffer, '\n')) = '\0';
	x = (int)strtol(buffer, NULL, 10);
	m -> games[4][0] = x;
      }
      else{
	m -> type = REQUEST_GAME;
	m -> game = x;
      }
      write(udp_id, m, sizeof(struct message));
			
      //Wait to receive game from server
      printf("Waiting for game to start\n*Annoying Hold Music*\n");
      read(socket_id, m, sizeof(struct message));
      if (m -> type == QUIT) {
	close(socket_id);
	close(udp_id);
	free(m);
	printf("Server closed\n");
	exit(0);
      }
			
      //Play game
      p = fork();
			
      if(p == 0)
	interface(m -> board, words, m -> games[1][m -> game], m -> id_num);
      else{
	wait(NULL);
	//empty word lists for next game
	x = 0;
	while(strlen(m -> original[x])){
	  strcpy(m -> original[x], "");
	  x++;
	}
	x = 0;
	while(strlen(m -> approved[x])){
	  strcpy(m -> approved[x], "");
	  x++;
	}
	x = 0;
	while(strlen(m -> overlap[x])){
	  strcpy(m -> overlap[x], "");
	  x++;
	}
	x = 0;
	while(strlen(m -> rejected[x])){
	  strcpy(m -> rejected[x], "");
	  x++;
	}
	words = copy_array(m -> id_num);
      }
			
      //flush stdin and buffer
      fflush(stdin);
      strcpy(buffer, "");
			
      //Send word list to server
			
      x = 0;
      while(strlen(words[x])){
	strcpy(m -> original[x], words[x]);
	x++;
      }
      m -> type = LIST_UP;
      write(udp_id, m, sizeof(struct message));
      printf("GAME OVER!\nWaiting for results...\n");
      //Wait for results
      read(socket_id, m, sizeof(struct message));
      if (m -> type == QUIT) {
	close(socket_id);
	close(udp_id);
	free(m);
	exit(0);
      }
      printf("Results In\n");
      //Display results
      printf("\nYour approved words:\n");
      x = 0;
      while(strlen(m -> approved[x])){
	printf("\t%d: %s\n", x + 1, m -> approved [x]);
	x++;
      }
      printf("\nYour rejected words:\n");
      x = 0;
      while(strlen(m -> rejected[x])){
	printf("\t%d: %s\n", x + 1, m -> rejected [x]);
	x++;
      }
      printf("\nYour overlap words:\n");
      x = 0;
      while(strlen(m -> overlap[x])){
	printf("\t%d: %s\n", x + 1, m -> overlap [x]);
	x++;
      }
      printf("\nScores:\n");
      x = 0;
      while(x < 10 && strlen(m -> players[x])){
	printf("\t%s: %d\n", m -> players [x], m -> scores [x]);
	x++;
      }
      printf("\nWinners:\n");
      x = 0;
      while(x < 10 && strlen(m -> players[x])){
	if(m -> scores[x] == m -> scores[0])
	  printf("\t%s\n", m -> players[x]);
	x++;
      }
      printf("\nLosers:\n");
      x--;
      p = x;
      // printf("Score of x: %d, score of p: %d\n",  m -> scores[x], m -> scores[p]);
      while(x >= 0 && m -> scores[x] == m -> scores[p]){
	printf("\t%s\n", m -> players[x]);
	x--;
      }
      printf("\n");
    }
  }
  close(socket_id);
  return 0;
	
}
