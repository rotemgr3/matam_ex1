CC = gcc
OBJS = chessSystem.o tournament.o game.o libmap.a chessSystemTestsExample.o
EXEC = chess_prog
DEBUG_FLAG = -std=c99 --pedantic-errors -Wall -Werror #-g to activate
COMP_FLAG = -std=c99 --pedantic-errors -Wall -Werror
LIBS = libmap.a

$(EXEC): $(OBJS)
	$(CC) $(COMP_FLAG) $(OBJS) -o $@
chessSystem.o : chessSystem.c map.h chessSystem.h game.h tournament.h
	$(CC) -c $(COMP_FLAG) $*.c
tournament.o : tournament.c game.h chessSystem.h map.h tournament.h
	$(CC) -c $(COMP_FLAG) $*.c
game.o : game.c game.h chessSystem.h map.h
	$(CC) -c $(COMP_FLAG) $*.c
chessSystemTestsExample.o : chessSystem.h test_utilities.h
	$(CC) -c $(COMP_FLAG) $*.c
clean : 
	rm -f chessSystem.o tournament.o game.o chessSystemTestsExample.o $(EXEC)
