#include <stdbool.h>  // for bool
#include "chess.h"
#include "board.h"
#include "move_generation.h"
#include "interface.h"


Piece* make_move(Move* move_ptr, Board* board_ptr) {
	Piece* piece_ptr = board_ptr->squares[move_ptr->from];
	Piece* target_ptr = board_ptr->squares[move_ptr->to];
	
	board_ptr->squares[piece_ptr->square] = 0;
	piece_ptr->square = move_ptr->to;
	board_ptr->squares[piece_ptr->square] = piece_ptr;

	if (move_ptr->type == CAPTURE) {
		target_ptr->alive = false;
		return target_ptr;
	}

	return 0;
}


void undo_move(Move* move_ptr, Board* board_ptr, Piece* captured_piece_ptr) {
	Piece* piece_ptr = board_ptr->squares[move_ptr->to];

	board_ptr->squares[piece_ptr->square] = captured_piece_ptr;
	piece_ptr->square = move_ptr->from;
	board_ptr->squares[piece_ptr->square] = piece_ptr;

	if (move_ptr->type == CAPTURE) {
		captured_piece_ptr->alive = true;
	}
}


void play_game() {
	char* fen_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	Board board = {};
	MoveList move_list = {};  
	setup_board(&board, fen_string);

	while (1) {
		// Generate all moves in a position for current player
		move_list.move_count = 0;  // Instead of clearing moves
		generate_pseudo_moves(&move_list, &board);
		find_legal_moves(&move_list, &board);

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
}
