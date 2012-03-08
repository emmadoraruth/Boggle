#define CONNECT 0
#define REQUEST_INFO 1
#define REQUEST_GAME 2
#define REQUEST_NEW_GAME 3
#define QUIT 4
#define LIST_UP 5
#define CLOSE_SERVER 6

struct message{
	int id_num, type, game, index, scores[10], games[5][8];
	char username[16];
	char board[5][5];
	char approved[40][17];
	char overlap[40][17];
	char rejected[20][17];
	char players[10][16];
	char original[60][17];
};
