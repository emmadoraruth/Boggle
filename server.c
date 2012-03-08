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

#include "search.h"
#include "board.h"
#include "message.h"
#define READ = 0
#define WRITE = 1


void subserver(int fromServer, int socket_client, int socket_id, int udp_id);
void compare(struct message ***clients, int game);
int compareHelper(char *word, struct message ***clients, int game, int h);
void spelling(struct message ***clients, int game, char **dic);
void existance(struct message ***clients, int game, int size, struct cube ****games);
void lengths(struct message ***clients, int game, int size);
void approve(struct message ***clients, int game);
void score(struct message ***clients, int game);
int scoreHelper(struct message ***clients, int game, int player);
void sort(int scores[], char **players, int num);

int main(){
	
  int toSubServers[160], client_ids[80], lastClient = 0, i, j, socket_id, k, x, y, games_list[5][8], socket_client, udp_id, udp_idb, r, f, w;
  time_t curr_time;
  socklen_t udp_length, socket_length;
  for (x = 0; x < 80; x++) 
    client_ids[x] = -1;
  char **dic = makeDic();
  char buffer[4];
  char **temp = (char **)malloc(5 * sizeof(char *));
  for(x = 0; x < 5; x++)
    temp[x] = (char *)malloc(sizeof(char));
  struct cube ****games = (struct cube ****)malloc(8 * sizeof(struct cube ***));
  struct message *m = (struct message *)malloc(sizeof(struct message));
  //Create an empty message to use when there are not the maximum number of clients
  struct message *empty = (struct message *)malloc(sizeof(struct message));
  strcpy(empty -> username, "");
  struct message ***clients = (struct message ***)malloc(8 * sizeof(struct message **));
  for(i = 0; i < 8; i++){
    clients[i] = (struct message **)malloc(10 * sizeof(struct message *));
    for(j = 0; j < 10; j++)
      clients[i][j] = (struct message *)malloc(sizeof(struct message));
  }
  struct tm *tm;
  time_t start;
	
  //zero out games_list
  for (x = 0; x < 5; x++) 
    for (y = 0; y < 8; y++) 
      games_list[x][y] = -1;
  //Do some initial housekeeping
  umask(0000);
	
  //create the pipe for listening
	
  struct sockaddr_in server;
	
  //make the server socket for reliable IPv4 traffic 
  socket_id = socket( AF_INET, SOCK_STREAM, 0);
	
	
  //TCP
  //set up the server socket struct
  //Use IPv4 
  server.sin_family = AF_INET;
	
  //This is the server, so it will listen to anything coming into the host computer
  server.sin_addr.s_addr = INADDR_ANY;
	
  //set the port to listen on, htons converts the port number to network format
  server.sin_port = htons(24611);
	
  //bind the socket to the socket struct
  i= bind( socket_id, (struct sockaddr *)&server, sizeof(server) );
	
  printf("TCP set up on server\n");
	
	
  //UDP	
  struct sockaddr_in udp, udpb;
	
  //make the server socket for reliable IPv4 traffic 
  udp_id = socket( AF_INET, SOCK_DGRAM, 0);
	
  //set up the server socket struct
  //Use IPv4 
  udp.sin_family = AF_INET;
	
  //This is the server, so it will listen to anything coming into the host computer
  udp.sin_addr.s_addr = INADDR_ANY;
	
  //set the port to listen on, htons converts the port number to network format
  udp.sin_port = htons(24612);
	
  //bind the socket to the socket struct
  r = bind( udp_id, (struct sockaddr *)&udp, sizeof(udp) );
	
  printf("UDP set up on server\n");
	
  printf("Enter 'q' or 'Q' to quit\n");
	
  f = fork();
	
  if (f != 0) {
    //make the server socket for reliable IPv4 traffic 
    udp_idb = socket( AF_INET, SOCK_DGRAM, 0);
		
		
    //set up the server socket struct
    //Use IPv4 
    udpb.sin_family = AF_INET;
		
    //Client will connect to address in argv[1], need to translate that IP address to binary
    inet_aton( "127.0.0.1", &(udpb.sin_addr) );
		
    //set the port to listen on, htons converts the port number to network format
    udpb.sin_port = htons(24612);
		
    //connect to the server
    w = connect(udp_idb, (struct sockaddr *)&udpb, sizeof(udpb));
  }
  while(f != 0){
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer [0] == 'q' || buffer[0] == 'Q') {
      m -> type = CLOSE_SERVER;
      write(udp_idb, m, sizeof(struct message));
      close(socket_id);
      close(udp_id);
      close(udp_idb);
      free(dic);
      free(temp);
      free(games);
      free(empty);
      free(clients);
      free(m);
      exit(0);
    }
  }

  strcpy(empty -> username, "");

	
  //Server runs forever
  while(1){
    udp_length = sizeof(udp);
    if ( recvfrom(udp_id, m, sizeof(struct message), 0, (struct sockaddr *)&udp, &udp_length)) {
			
      printf("Message from: %d\n", m->id_num);
			
      //if mesage is a new connection, set up subserver...
      if ( m->type == CONNECT ) {
	printf("Server connecting to client\n");
	i =  listen( socket_id, 1 );
	socket_length = sizeof(server);
	socket_client = accept(socket_id, (struct sockaddr *)&server, &socket_length);
	//create an unnamed pipe to the subserver
	lastClient = 0;
	while (client_ids[lastClient] != -1) 
	  lastClient++;
	assert( pipe(&toSubServers[lastClient * 2]) == 0 );
	client_ids[lastClient] = lastClient;
	m->id_num = lastClient;
				
	i = fork();
	//initialize the subserver if child process
	if ( i == 0 ) {
					
	  subserver(toSubServers[lastClient * 2], socket_client, socket_id, udp_id);
	  exit(0);
	} //close child-only instructions
				
	//only thing the parent needs to do is increment lastClient
	else{
	  sleep(1);
	  //printf("Writing to: %d\n", toSubServers[lastClient * 2]);
	  write(toSubServers[lastClient * 2 + 1], m, sizeof(struct message));
	  close(socket_client);
	}//close parent-only instructions  
	printf("Done Connecting\n");
      } //close connect condition
			
			
      //If the message is anything else
      else{
	printf("Non-connection message\n");
	if (m -> type == QUIT) {
	  write(toSubServers[m -> id_num * 2 + 1], m, sizeof(struct message));
	  client_ids[m -> id_num] = -1;
	  close(toSubServers[m -> id_num * 2]);
	  close(toSubServers[m -> id_num * 2 + 1]);
	}
				
	else if(m -> type == REQUEST_INFO){
	  // printf("Request for info\n");
	  //Fill message with games data
	  for(y = 0; y < 8; y ++){
	    for(x = 0; x < 2; x++)
	      m -> games[x][y] = games_list[x][y];
	    k = 0;
	    curr_time = (int)NULL;
	    curr_time = time(NULL);
	    //Find out how long until the game starts
	    tm = NULL;
	    tm = localtime(&curr_time);
	    k += (games_list[2][y] - (tm -> tm_hour)) * 3600;
	    k += (games_list[3][y] - (tm -> tm_min)) * 60;
	    k += (games_list[4][y] - (tm -> tm_sec));
	    m -> games[2][y] = (int)k/3600;
	    m -> games[3][y] = (int)(k % 60)/60;
	    m -> games[4][y] = (int)k % 3600;
	  }
	  //Send to appropriate subserver
	  write(toSubServers[m -> id_num * 2 + 1], m, sizeof(struct message));
	}
				
	else if(m -> type == REQUEST_GAME){
	  //Update number of players in game
	  games_list[0][m -> game] = games_list[0][m -> game] + 1;
	  //printf("Num players: %d", games_list[0][m -> game]);
	  //Fill message with appropriate board
	  temp = board_to_chars(games[m -> game], (m -> games)[1][0]);
	  for(x = 0; x < 5; x++)
	    for(y = 0; y < 5; y ++)
	      m -> board[x][y] = temp[x][y];
	  m -> games[2][m -> game] = games_list[2][m -> game];
	  m -> games[3][m -> game] = games_list[3][m -> game];
	  m -> games[4][m -> game] = games_list[4][m -> game];
	  //Send to appropriate subserver
	  write(toSubServers[m -> id_num * 2 + 1], m, sizeof(struct message));
	}
				
	else if(m -> type == REQUEST_NEW_GAME){
	  //Create a new game
	  k = 0;
	  while(games_list[0][k] != -1)
	    k++;
	  m -> game = k;
	  games_list[0][k] = (m -> games)[0][0];
	  games_list[1][k] = (m -> games)[1][0];
	  (m -> games)[1][k] = (m -> games)[1][0];
	  //Find time, according to server's clock, when new game will start and store it
	  x = (int)time(0);
	  x += (m -> games)[2][0] * 3600;
	  x += (m -> games)[3][0] * 60;
	  x += (m -> games)[4][0];
	  start = (time_t)x;
	  tm = localtime(&start);
	  games_list[2][k] = (int)(tm -> tm_hour);
	  games_list[3][k] = (int)(tm -> tm_min);
	  games_list[4][k] = (int)(tm -> tm_sec);
	  m -> games[2][k] = (int)(tm -> tm_hour);
	  m -> games[3][k] = (int)(tm -> tm_min);
	  m -> games[4][k] = (int)(tm -> tm_sec);
	  if((m -> games)[1][0] == 4){
	    printf("Initializing 4\n");
	    games[k] = initialize4();
	    //print4(games[k]);
	  }
	  else{
	    printf("Initializing 5\n");
	    games[k] = initialize5();
	    //print5(games[k]);
	  }
	  //Fill message with appropriate board
	  temp = board_to_chars(games[k], m -> games[1][0]);
	  for(x = 0; x < 5; x++)
	    for(y = 0; y < 5; y ++)
	      m -> board[x][y] = temp[x][y];
	  //Send to appropriate subserver
	  write(toSubServers[m -> id_num * 2 + 1], m, sizeof(struct message));
	}
				
	else if (m ->type == CLOSE_SERVER){
	  printf("Quitting\n");
	  m -> type = QUIT;
	  for (w = 0; w < 80; w++){
	    if (client_ids [w] >= 0) {
	      printf("Closing client: %d\n", w);
	      write(toSubServers[w * 2 + 1], m, sizeof(struct message));
	    }
	  }
	  close(socket_id);
	  close(udp_id);
	  free(dic);
	  free(temp);
	  free(games);
	  free(empty);
	  free(clients);
	  free(m);
	  exit(0);
	}
				
	else if(m -> type == LIST_UP){
					
	  printf("Received a list from%s\n", m -> username);
	  //Go to the correct client slot
	  //If this is not the first results
	  /*
	    k = 0;
	    if(clients[m -> game][0]){
	    while(strlen(clients[m -> game][k] -> username))
	    k++;
	    }
	  */
	  games_list[0][m -> game] = games_list[0][m -> game] - 1;
										
	  x = 0;
	  while(strlen(m -> original[x])){
	    strcpy(clients[m -> game][games_list[0][m -> game]] -> original[x], m -> original[x]);
	    x++;
	  }
	  strcpy(clients[m -> game][games_list[0][m -> game]] -> username, m -> username);
	  clients[m -> game][games_list[0][m -> game]] -> game = m -> game;
	  clients[m -> game][games_list[0][m -> game]] -> id_num = m -> id_num;
					
	  //If this is the last list:
	  if(games_list[0][m -> game] == 0){
	    //printf("This is the last list of game: %d\n", m -> game);
						
	    //Check lists against each other and fill overlap
	    compare(clients, m -> game);
						
	    //Check lists against board and start filling rejected
	    existance(clients, m -> game, games_list[1][m -> game], games);
				
	    //Check lists for length of words and continue filling rejected
	    lengths(clients, m -> game, games_list[1][m -> game]);

	    //Check lists against dictionary and finish filling rejected
	    spelling(clients, m -> game, dic);
	    x = 0;
				
	    //Fills approved
	    approve(clients, m -> game);
						
	    //Calculate and enter results
	    score(clients, m -> game);
						
	    //Send to appropriate subservers
	    k = 0;
	    while (strlen(clients[m -> game][k] -> username)){
	      write(toSubServers[clients[m -> game][k] -> id_num * 2 + 1], clients[m -> game][k], sizeof(struct message));
	      k++;
	    }
						
	    //Remove game
	    for (k = 0; k < 5; k++)
	      games_list[k][m -> game] = -1;
	    for(k = 0; k < 10; k++){
	      clients[m -> game][k] = (struct message *)malloc(sizeof(struct message));
	      strcpy(clients[m -> game][k] -> username, "");
	    }
	  }
					
	}
				
      } //Close mesage other than connect
			
    } //Close read > 0
		
  } //Close forever loop
  return 0;
	
}



/*======== void subserver() ==========
 Inputs:  int fromServer
 char * buffer  
 Returns: 
 
 Reads messages from a pipe to the main server, and writes them to its associated client
 
 12/06/09 14:43:51
 jdyrlandweaver
 ====================*/
void subserver(int fromServer, int socket_client, int socket_id, int udp_id) {
	
  struct message *m = (struct message *)malloc(sizeof(struct message));
  time_t now;
  struct tm * tm;
  int wait;
  printf("Subserver running: %d\n", fromServer);
	
  //listen to the main server for a message
  while (read(fromServer, m, sizeof(struct message)) > 0 && m -> type != QUIT) {
    if(m -> type == REQUEST_NEW_GAME || m -> type == REQUEST_GAME){
      now = time(0);
      tm = localtime(&now);		
      wait = 0;
      wait += ((m -> games[2][m -> game]) - (tm -> tm_hour)) * 3600;
      wait += ((m -> games[3][m -> game]) - (tm -> tm_min)) * 60;
      wait += ((m -> games[4][m -> game]) - (tm -> tm_sec));
      //printf("Waiting: % d\n", wait);
      sleep(wait);
    }
    //printf("Subserver writing type: %d\n", m -> type);
    //process the data and write it back
    write(socket_client, m, sizeof(struct message));
  }  
	
  write(socket_client, m, sizeof(struct message));
  close(socket_client);
  close(socket_id);
  close(udp_id);
  exit(0);
}

//Returns whether a word is in a list
int compareHelper(char *word, struct message ***clients, int game, int h){
  int f = 0;
  while(strlen(clients[game][h] -> original[f])){
    //printf("Comparing %s, with %s\n", word, clients[game][h] -> original[f]);
    //printf("Compared %d\n", strncmp(clients[game][h] -> original[f], "~", 1));
    if(strncmp(clients[game][h] -> original[f], "~", 1)){
      //printf("Test2\n");
      if(!strcmp(word, clients[game][h] -> original[f])){
	//printf("I disapprove of: %s\n", clients[game][h] -> original[f]);
	clients[game][h] -> original[f][0] = '~';
	return 1;
      }
    }
    f++;
  }
  return 0;
}

//Checks all lists against each other
//Puts shared words into appropriate clients' overlap lists
//Marks shared words as checked by changing the first letter to a tilda ('~') on original lists
void compare(struct message ***clients, int game){
  int f = 0, g = 0, h = 1, j = 0, k = 0;
  while (strlen(clients[game][f] -> username)){
    clients[game][f] -> index = 0;
    f++;
  }

  f = 0;
  //f is keeping track of which list we're checking against
  while(strlen(clients[game][f + 1] -> username)){
    //printf("New list\n");
    g = 0;
    //g is keeping track of which word we're checking
    while(strlen(clients[game][f] -> original[g])){
      //printf("New word\n");
      h = f + 1;
      //h is keeping track of which list f is checking against
      k = 0;
      while(strlen(clients[game][h] -> username)){
	//printf("New other list\n");
	//printf("\nComparing word %s of player %d against player %d\n", clients[game][f] -> original[g], f, h);
	j = compareHelper(clients[game][f] -> original[g], clients, game, h);
	//j is true if g is an overlap in h
	if(j){
	  strcpy(clients[game][h] -> overlap[clients[game][h] -> index], clients[game][f] -> original[g]);
	  clients[game][h] -> index = clients[game][h] -> index + 1;
	  //printf("Setting k for %s in player %d\n", clients[game][f]->original[g], f);
	  k = 1;
	} //Closes j's if statment
	h++;
      } //Closes h's while loop
      if(k){
	strcpy(clients[game][f] -> overlap[clients[game][f] -> index], clients[game][f] -> original[g]);
	clients[game][f] -> index = clients[game][f] -> index + 1;
	clients[game][f] -> original[g][0] = '~';
      } //Closes k's if statement
      g++;
    } //Closes g's for loop
    //printf("Incrementing f\n");
    f++;
  } //Closes f's while loop
} //Closes entire function

//Checks the lists of all players in game game against dictionary dic
void spelling(struct message ***clients, int game, char **dic){
  int f = 0, w = 0;
  while(strlen(clients[game][f] -> username)){
    w = 0;
    while(strlen(clients[game][f] -> original[w])){
      //printf("Checking %s\n", clients[game][f] -> original[w]);
      if(strncmp(clients[game][f] -> original[w], "~", 1) && !search(clients[game][f] -> original[w], dic)){
	strcpy(clients[game][f]->rejected[clients[game][f]->index], clients[game][f] -> original[w]);
	clients[game][f] -> index = clients[game][f] -> index + 1;
	clients[game][f] -> original[w][0] = '~';
      }
      w++;
    }
    f++;
  }
}

//Checks the lists of all players in game game against the board
void existance(struct message ***clients, int game, int size, struct cube ****games){
  int f = 0, w = 0;
  //f keeps track of which client we're checking
  if(size == 4)
    print4(games[game]);
  else
    print5(games[game]);
  while(strlen(clients[game][f] -> username)){
    clients[game][f] -> index = 0;
    //w keeps track of which word we're checking
    w = 0;
    while(strlen(clients[game][f] -> original[w])){
      //printf("Checking: %s\n", clients[game][f] -> original[w]);
      if(strncmp(clients[game][f] -> original[w], "~", 1) && !on_board(clients[game][f] -> original[w], games[game], size)){
	strcpy(clients[game][f] -> rejected[clients[game][f] -> index], clients[game][f] -> original[w]);
	clients[game][f] -> index = clients[game][f] -> index + 1;
	clients[game][f] -> original[w][0] = '~';
      }
      w++;
    }
    f++;
  }
  //printf("\n");
}

//Checks the lists of all players in game game for minimum length
void lengths(struct message ***clients, int game, int size){
  int f = 0, w = 0;
  while(strlen(clients[game][f] -> username)){
    while(strlen(clients[game][f] -> original[w])){
      if(strncmp(clients[game][f] -> original[w], "~", 1) && !check_length(clients[game][f] -> original[w], size)){
	strcpy(clients[game][f] -> rejected[clients[game][f] -> index], clients[game][f] -> original[w]);
	//printf("Rejcted: %s\n", clients[game][f] -> original[w]);
	clients[game][f] -> index = clients[game][f] -> index + 1;
	clients[game][f] -> original[w][0] = '~';
      }
      w++;
    }
    w = 0;
    f++;
  }
  /*f = 0;
    w = 0;
    while(strlen(clients[game][f] -> username)){
    printf("%s's Rejected Words:\n", clients[game][f] -> username);
    while(strlen(clients[game][f] -> rejected[w])){
    printf("%s\n", clients[game][f] -> rejected[w]);
    w++;
    }
    w = 0;
    f++;
    }*/
}

//Puts all acceptable words for each player into their approved lists in their messages
void approve(struct message ***clients, int game){
  int f = 0, w = 0;
  //f tracks which client's words are being approved
  while(strlen(clients[game][f] -> username)){
    w = 0;
    clients[game][f] -> index = 0;
    //w tracks which word is being approved
    while(strlen(clients[game][f] -> original[w])){
      if(strncmp(clients[game][f] -> original[w], "~", 1)){
	strcpy(clients[game][f] -> approved[clients[game][f] -> index], clients[game][f] -> original[w]);
	clients[game][f] -> index = clients[game][f] -> index + 1;
      }
      w++;
    }
    f++;
  }
}

//Calculates and returns the score for the two-dimensional char array (a player's word list) words
int scoreHelper(struct message ***clients, int game, int player){
  int f = 0, total = 0;
  //printf("\nMade it to helper function!\n");
  while(strlen(clients[game][player] -> approved[f])){
    //printf("\nMade it to while loop!\n");
    if(strlen(clients[game][player] -> approved[f]) <= 4)
      total++;
    else if(strlen(clients[game][player] -> approved[f]) == 5)
      total += 2;
    else if(strlen(clients[game][player] -> approved[f]) == 6)
      total += 3;
    else if(strlen(clients[game][player] -> approved[f]) == 7)
      total += 5;
    else
      total += 11;
    f++;
  }
  return total;
}

//Scores the approved lists of words for all players in game game
//Sorts the players in order of descending scores
//Adds the lists of scores and players to the message going to each client
void score(struct message ***clients, int game){
  int f = 0, all_scores[10], x;
  char **all_players = (char **)malloc(10 * sizeof(char *));
  for(x = 0; x < 16; x++)
    all_players[x] = (char *)malloc(16 * sizeof(char));
  while(strlen(clients[game][f] -> username)){
    all_scores[f] = scoreHelper(clients, game, f);
    strncpy(all_players[f], clients[game][f] -> username, 16);
    f++;
  }
  sort(all_scores, all_players, f);
  f = 0;
  while(strlen(clients[game][f] -> username)){
    x = 0;
    while(strlen(clients[game][x] -> username)){
      clients[game][f] -> scores[x] = all_scores[x];
      strcpy(clients[game][f] -> players[x], all_players[x]);
      x++;
    }
    f++;
  }
}

//Sorts the scores from highest to lowest
//Rearranges the players' usernames in order of their scores, highest to lowest
void sort(int scores[], char **players, int num){
  int i, j, temps;
  //for(i = 0; i < num; i++)
  //printf("%s has score %d\n", players[i], scores[i]);
  char *tempp = (char *)malloc(16 * sizeof(char));
  for(i = 0; i < num - 1; i++)
    for(j = i + 1; j < num; j++)
      if(scores[i] < scores[j]){
	temps = scores[i];
	scores[i] = scores[j];
	scores[j] = temps;
	strcpy(tempp, players[i]);
	strcpy(players[i], players[j]);
	strcpy(players[j], tempp);
      }
  //for(i = 0; i < num; i++)
  //printf("%s has score %d\n", players[i], scores[i]);
}
