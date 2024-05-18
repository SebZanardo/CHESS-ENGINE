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


void perform_castle(MoveType move_type, Board* board_ptr) {
	// Only need to teleport rook to other side of king
	if (move_type == CASTLE_KINGSIDE && board_ptr->current_turn == WHITE) {
		board_ptr->squares[F1] = board_ptr->squares[H1];
		board_ptr->squares[H1] = 0;
		board_ptr->squares[F1]->square = F1;
	}
	else if (move_type == CASTLE_QUEENSIDE && board_ptr->current_turn == WHITE) {
		board_ptr->squares[D1] = board_ptr->squares[A1];
		board_ptr->squares[A1] = 0;
		board_ptr->squares[D1]->square = D1;
	}
	else if (move_type == CASTLE_KINGSIDE && board_ptr->current_turn == BLACK) {
		board_ptr->squares[F8] = board_ptr->squares[H8];
		board_ptr->squares[H8] = 0;
		board_ptr->squares[F8]->square = F8;
	}
	else if (move_type == CASTLE_QUEENSIDE && board_ptr->current_turn == BLACK) {
		board_ptr->squares[D8] = board_ptr->squares[A8];
		board_ptr->squares[A8] = 0;
		board_ptr->squares[D8]->square = D8;
	}
}


void unperform_castle(MoveType move_type, Board* board_ptr) {
	// Only need to un-teleport rook back to starting square
	if (move_type == CASTLE_KINGSIDE && board_ptr->current_turn == WHITE) {
		board_ptr->squares[H1] = board_ptr->squares[F1];
		board_ptr->squares[F1] = 0;
		board_ptr->squares[H1]->square = H1;
	}
	else if (move_type == CASTLE_QUEENSIDE && board_ptr->current_turn == WHITE) {
		board_ptr->squares[A1] = board_ptr->squares[D1];
		board_ptr->squares[D1] = 0;
		board_ptr->squares[A1]->square = A1;
	}
	else if (move_type == CASTLE_KINGSIDE && board_ptr->current_turn == BLACK) {
		board_ptr->squares[H8] = board_ptr->squares[F8];
		board_ptr->squares[F8] = 0;
		board_ptr->squares[H1]->square = H1;
	}
	else if (move_type == CASTLE_QUEENSIDE && board_ptr->current_turn == BLACK) {
		board_ptr->squares[A8] = board_ptr->squares[D8];
		board_ptr->squares[D8] = 0;
		board_ptr->squares[A8]->square = A8;
	}
}


Piece* make_move(Move* move_ptr, Board* board_ptr) {
	Piece* piece_ptr = board_ptr->squares[move_ptr->from];
	Piece* target_ptr = board_ptr->squares[move_ptr->to];
	
	board_ptr->squares[piece_ptr->square] = 0;
	piece_ptr->square = move_ptr->to;
	board_ptr->squares[piece_ptr->square] = piece_ptr;

	// Perform special moves
	perform_promotion(move_ptr->type, piece_ptr);
	Piece* ep_target = perform_en_passant(move_ptr, board_ptr);
	if (ep_target) {
		target_ptr = ep_target;
	}
	perform_castle(move_ptr->type, board_ptr);

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

	// Unperform special moves
	unperform_promotion(move_ptr->type, piece_ptr);
	unperform_en_passant(move_ptr, board_ptr, captured_piece_ptr);
	unperform_castle(move_ptr->type, board_ptr);

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


void update_castling_rights(Move* move_ptr, Board* board_ptr) {
	// If move was castling set rights to false
	if (move_ptr->type == CASTLE_KINGSIDE || move_ptr->type == CASTLE_QUEENSIDE) {
		board_ptr->castling_rights[board_ptr->current_turn][KINGSIDE] = false;
		board_ptr->castling_rights[board_ptr->current_turn][QUEENSIDE] = false;
		return;
	}

	if (board_ptr->current_turn == WHITE) {
		// If player moved rook or king and can still castle
		if (board_ptr->castling_rights[WHITE][KINGSIDE]) {
			if (move_ptr->from == E1 || move_ptr->from == H1) {
				board_ptr->castling_rights[WHITE][KINGSIDE] = false;
			}
		}
		if (board_ptr->castling_rights[WHITE][QUEENSIDE]) {
			if (move_ptr->from == E1 || move_ptr->from == A1) {
				board_ptr->castling_rights[WHITE][QUEENSIDE] = false;
			}
		}

		// If player captured opponents rook and they can still castle
		if (board_ptr->castling_rights[BLACK][KINGSIDE]) {
			if (move_ptr->to == H8) {
				board_ptr->castling_rights[BLACK][KINGSIDE] = false;
			}
		}
		if (board_ptr->castling_rights[BLACK][QUEENSIDE]) {
			if (move_ptr->to == A8) {
				board_ptr->castling_rights[BLACK][QUEENSIDE] = false;
			}
		}
	}
	else {
		// If player moved rook or king and can still castle
		if (board_ptr->castling_rights[BLACK][KINGSIDE]) {
			if (move_ptr->from == E8 || move_ptr->from == H8) {
				board_ptr->castling_rights[BLACK][KINGSIDE] = false;
			}
		}
		if (board_ptr->castling_rights[BLACK][QUEENSIDE]) {
			if (move_ptr->from == E8 || move_ptr->from == A8) {
				board_ptr->castling_rights[BLACK][QUEENSIDE] = false;
			}
		}

		// If player captured opponents rook and they can still castle
		if (board_ptr->castling_rights[WHITE][KINGSIDE]) {
			if (move_ptr->to == H1) {
				board_ptr->castling_rights[WHITE][KINGSIDE] = false;
			}
		}
		if (board_ptr->castling_rights[WHITE][QUEENSIDE]) {
			if (move_ptr->to == A1) {
				board_ptr->castling_rights[WHITE][QUEENSIDE] = false;
			}
		}
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

		// Update board state
		update_en_passant_target(selected_move_ptr, &board);
		update_castling_rights(selected_move_ptr, &board);

		// Change turn
		switch_current_turn(&board);
	}
}
