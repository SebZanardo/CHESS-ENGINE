// gcc -o out main.c chess.c interface.c
#include "chess.h"
#include "interface.h"


int main(void) {
	char* fen_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	Board board = {};
	MoveList move_list = {};
	setup_board(&board, fen_string);

	while (1) {
		// Generate all moves in a position for current player
		move_list.move_count = 0;
		generate_pseudo_moves(&move_list, &board);

		// Display board and moves
		print_board(&board);
		print_board_details(&board);
		print_move_list(&move_list);

		// Check for gameover
		if (move_list.move_count == 0) {
			break;
		}

		// Get move
		int i = get_move_index(&move_list);

		// Make move
		make_move(&move_list.moves[i], &board);

		// Change turn
		switch_current_turn(&board);
	}

	return 0;
}
