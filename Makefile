all : server client
server : server.o search.o cube.o board.o board4.o board5.o
	gcc -o server server.o search.o cube.o board.o board4.o board5.o
client : client.o board.o board5.o board4.o cube.o game.o
	gcc -o client -lncurses client.o board.o board4.o board5.o cube.o game.o
client.o : message.h board.h client.c
	gcc -c client.c
server.o : server.c message.h search.h board.h search.o
	gcc -c server.c
search.o : search.c search.h
	gcc -c search.c
game.o : game.c game.h
	gcc -c -lncurses game.c
board.o : board.c board.h cube.c cube.h server.o
	gcc -c board.c
board4.o : board4.c board.c board.h cube.h
	gcc -c board4.c
board5.o : board5.c board.c board.h cube.h
	gcc -c board5.c
cube.o : cube.c cube.h
	gcc -c cube.c
clean :
	rm *.o
.PHONY : clean