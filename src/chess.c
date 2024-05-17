#include <stdbool.h>  // for bool
#include "chess.h"
#include "board.h"
#include "move_generation.h"
#include "interface.h"


void perform_promotion(MoveType move_type, Piece* piece_ptr) {
	if (move_type == PROMOTION_KNIGHT || move_type == CAPTURE_PROMOTION_KNIGHT) {
		piece_ptr->type = KNIGHT;
	}
	else if (move_type == PROMOTION_BISHOP || move_type == CAPTURE_PROMOTION_BISHOP) {
		piece_ptr->type = BISHOP;
	}
	else if (move_type == PROMOTION_ROOK || move_type == CAPTURE_PROMOTION_ROOK) {
		piece_ptr->type = ROOK;
	}
	else if (move_type == PROMOTION_QUEEN || move_type == CAPTURE_PROMOTION_QUEEN) {
		piece_ptr->type = QUEEN;
	}
}


void unperform_promotion(MoveType move_type, Piece* piece_ptr) {
	if (
	move_type == PROMOTION_KNIGHT || move_type == CAPTURE_PROMOTION_KNIGHT || 
	move_type == PROMOTION_BISHOP || move_type == CAPTURE_PROMOTION_BISHOP ||
	move_type == PROMOTION_ROOK || move_type == CAPTURE_PROMOTION_ROOK ||
	move_type == PROMOTION_QUEEN || move_type == CAPTURE_PROMOTION_QUEEN
	) {
		piece_ptr->type = PAWN;
	}
}


Piece* perform_en_passant(Move* move_ptr, Board* board_ptr) {
	if (move_ptr->type != EN_PASSANT) {
		return 0;
	}
	
	// Find square where double pushed pawn is
	Square ep_square = move_ptr->to;
	ep_square += board_ptr->current_turn == WHITE ? -8 : 8;

	// Remove double pushed pawn from board
	Piece* captured_ep_piece_ptr = board_ptr->squares[ep_square];
	board_ptr->squares[ep_square] = 0;

	return captured_ep_piece_ptr;
}


void unperform_en_passant(Move* move_ptr, Board* board_ptr, Piece* captured_ep_piece_ptr) {
	if (move_ptr->type != EN_PASSANT) {
		return;
	}
	
	// Find square where double pushed pawn is
	Square ep_square = move_ptr->to;
	ep_square += board_ptr->current_turn == WHITE ? -8 : 8;

	// Place double pushed pawn back on board
	board_ptr->squares[ep_square] = captured_ep_piece_ptr;
}


Piece* make_move(Move* move_ptr, Board* board_ptr) {
	Piece* piece_ptr = board_ptr->squares[move_ptr->from];
	Piece* target_ptr = board_ptr->squares[move_ptr->to];
	
	board_ptr->squares[piece_ptr->square] = 0;
	piece_ptr->square = move_ptr->to;
	board_ptr->squares[piece_ptr->square] = piece_ptr;


	// Perform special moves
	/* TODO: Perform castle */
	
	// Perform pawn promotion
	perform_promotion(move_ptr->type, piece_ptr);

	// Perform en passant
	Piece* ep_target = perform_en_passant(move_ptr, board_ptr);
	if (ep_target) {
		target_ptr = ep_target;
	}

	// Set captured piece to dead
	if (target_ptr) {
		target_ptr->alive = false;
		return target_ptr;
	}

	return 0;
}


void undo_move(Move* move_ptr, Board* board_ptr, Piece* captured_piece_ptr) {
	Piece* piece_ptr = board_ptr->squares[move_ptr->to];

	if (move_ptr->type == EN_PASSANT) {
		// Let unperform_en_passant handle placement of captured piece
		board_ptr->squares[piece_ptr->square] = 0;
	}
	else {
		board_ptr->squares[piece_ptr->square] = captured_piece_ptr;
	}

	piece_ptr->square = move_ptr->from;
	board_ptr->squares[piece_ptr->square] = piece_ptr;

	// Perform special moves
	/* TODO: Unperform castle */
	unperform_promotion(move_ptr->type, piece_ptr);
	unperform_en_passant(move_ptr, board_ptr, captured_piece_ptr);

	// Set captured piece to alive
	if (captured_piece_ptr) {
		captured_piece_ptr->alive = true;
	}
}


void update_en_passant_target(Move* move_ptr, Board* board_ptr) {
	board_ptr->en_passant_target = NONE;
	if (move_ptr->type == DOUBLE_PAWN_PUSH) {
		// Find square as if pawn had only moved once
		Square ep_square = move_ptr->to;
		ep_square += board_ptr->current_turn == WHITE ? -8 : 8;
		board_ptr->en_passant_target = ep_square;
	}
}


void play_game() {
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	Board board = {};
	MoveList move_list = {};  
	setup_board(&board, fen);

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
		Move* selected_move_ptr = &move_list.moves[i];

		// Make move
		make_move(selected_move_ptr, &board);

		// TODO: Update castling rights
		// - if move was a castling set rights to false
		// - if moved king set castle rights to false
		// - if rook captured or moved set castle rights to false for side

		update_en_passant_target(selected_move_ptr, &board);

		// Change turn
		switch_current_turn(&board);
	}
}
