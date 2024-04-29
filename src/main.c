// gcc -o out main.c chess.c interface.c
#include "chess.h"
#include "interface.h"


int main(void) {
	Board board;
	print_board(board);
	return 0;
}
