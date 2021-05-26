CC = gcc
OBJS = chess.o tournament.o game.o chessSystemTestsExample.o libmap.a
EXEC = chess
DEBUG_FLAG = -std=c99 --pedantic-errors -Wall -Werror #-g to activate
COMP_FLAG = -std=c99 --pedantic-errors -Wall -Werror


$(EXEC): $(OBJS)
	$(CC) $(COMP_FLAG) $(OBJS) -o $@
chess.o : chessSystem.c map.h chessSystem.h game.h tournament.h
	$(CC) -c $(COMP_FLAG) chessSystem.c -o chess.o
tournament.o : tournament.c game.h chessSystem.h map.h tournament.h
	$(CC) -c $(COMP_FLAG) $*.c
game.o : game.c game.h chessSystem.h map.h
	$(CC) -c $(COMP_FLAG) $*.c
chessSystemTestsExample.o : tests/chessSystemTestsExample.c chessSystem.h test_utilities.h
	$(CC) -c $(COMP_FLAG) tests/$*.c
clean : 
	rm -f chess.o tournament.o game.o chessSystemTestsExample.o $(EXEC)

