// gcc -o out main.c chess.c interface.c
#include "chess.h"
#include "interface.h"


int main(void) {
	char* fen_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	Board board = {};

	setup_board(&board, fen_string);

	print_board(&board);
	print_board_details(&board);

	return 0;
}
