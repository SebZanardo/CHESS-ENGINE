#include <stdbool.h>  // for bool
#include "chess.h"
#include "board.h"


// Pawns needs custom logic for each move
int knight_moves[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, -1}, {-2, 1}};
int king_moves[8][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int bishop_directions[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int rook_directions[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
// Queens directions are just rook_directions + bishop_directions


void set_move(Move* move_ptr, Square from, Square to, MoveType type) {
	move_ptr->from = from;
	move_ptr->to = to;
	move_ptr->type = type;
}


void get_set_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr, int moves[][2], int moves_len) {
	int file = index_to_file(piece_ptr->square);
	int rank = index_to_rank(piece_ptr->square);

	int new_file, new_rank;
	int target_square;
	Piece* target_piece_ptr;
	for (int i = 0; i < moves_len; i++) {
		new_file = file + moves[i][0];
		new_rank = rank + moves[i][1];

		if (!inside_board(new_file, new_rank)) { continue; }

		target_square = coordinate_to_index(new_file, new_rank);
		target_piece_ptr = board_ptr->squares[target_square];

		if (!target_piece_ptr) {
			Move move = {};
			set_move(&move, piece_ptr->square, target_square, QUIET_MOVE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
		}
		else if (piece_ptr->colour != target_piece_ptr->colour) {
			Move move = {};
			set_move(&move, piece_ptr->square, target_square, CAPTURE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
		}
	}
}


void get_sliding_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr, int directions[][2], int directions_len) {
	int file = index_to_file(piece_ptr->square);
	int rank = index_to_rank(piece_ptr->square);

	int new_file, new_rank;
	int target_square;
	Piece* target_piece_ptr;
	for (int i = 0; i < directions_len; i++) {
		new_file = file + directions[i][0];
		new_rank = rank + directions[i][1];

		for (int j = 0; j < 7; j++) {
			if (!inside_board(new_file, new_rank)) { break; }
			target_square = coordinate_to_index(new_file, new_rank);
			target_piece_ptr = board_ptr->squares[target_square];

			if (!target_piece_ptr) {
				Move move = {};
				set_move(&move, piece_ptr->square, target_square, QUIET_MOVE);
				move_list_ptr->moves[move_list_ptr->move_count++] = move;
			}
			else {
				break;
			}
			new_file += directions[i][0];
			new_rank += directions[i][1];
		}

		if (!inside_board(new_file, new_rank)) { continue; }
		target_square = coordinate_to_index(new_file, new_rank);
		target_piece_ptr = board_ptr->squares[target_square];
		if (target_piece_ptr && piece_ptr->colour != target_piece_ptr->colour) {
			Move move = {};
			set_move(&move, piece_ptr->square, target_square, CAPTURE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
		}
	}
}


void add_pawn_promotions(MoveList* move_list_ptr, Board* board_ptr, Square from, Square to) {
	Move move = {};
	set_move(&move, from, to, PROMOTION_QUEEN);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
	set_move(&move, from, to, PROMOTION_ROOK);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
	set_move(&move, from, to, PROMOTION_BISHOP);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
	set_move(&move, from, to, PROMOTION_KNIGHT);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
}


void add_pawn_capture_promotions(MoveList* move_list_ptr, Board* board_ptr, Square from, Square to) {
	Move move = {};
	set_move(&move, from, to, CAPTURE_PROMOTION_QUEEN);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
	set_move(&move, from, to, CAPTURE_PROMOTION_ROOK);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
	set_move(&move, from, to, CAPTURE_PROMOTION_BISHOP);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
	set_move(&move, from, to, CAPTURE_PROMOTION_KNIGHT);
	move_list_ptr->moves[move_list_ptr->move_count++] = move;
}


void get_pawn_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	int forward = piece_ptr->colour == WHITE ? 1 : -1;

	int file = index_to_file(piece_ptr->square);
	int rank = index_to_rank(piece_ptr->square);

	int new_file, new_rank;

	// Don't need to check if inside board for pawn moves, only captures. 

	// Move forward one
	new_file = file;
	new_rank = rank + forward;

	int target_square = coordinate_to_index(new_file, new_rank);
	Piece* target_piece_ptr = board_ptr->squares[target_square];
	if (!target_piece_ptr) {
		// Check for pawn promotion
		if (piece_ptr->colour == WHITE && new_rank == 7 || piece_ptr->colour == BLACK && new_rank == 0){
			add_pawn_promotions(move_list_ptr, board_ptr, piece_ptr->square, target_square);
		}
		else {
			Move move = {};
			set_move(&move, piece_ptr->square, target_square, QUIET_MOVE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
			
			// Check for double pawn push
			bool on_starting_square = false;
			if (piece_ptr->colour == WHITE && rank == 1 || piece_ptr->colour == BLACK && rank == 6) {
				on_starting_square = true;
			}
			if (on_starting_square) {
				new_rank += forward;
				target_square = coordinate_to_index(new_file, new_rank);
				target_piece_ptr = board_ptr->squares[target_square];
				if (!target_piece_ptr) {
					Move move = {};
					set_move(&move, piece_ptr->square, target_square, DOUBLE_PAWN_PUSH);
					move_list_ptr->moves[move_list_ptr->move_count++] = move;
				}
			}
		}
	}

	// Captures
	new_rank = rank + forward;

	new_file = file + 1;
	if (inside_board(new_file, new_rank)) {
		target_square = coordinate_to_index(new_file, new_rank);
		target_piece_ptr = board_ptr->squares[target_square];
		if (target_piece_ptr && piece_ptr->colour != target_piece_ptr->colour) {
			// Check for pawn capture promotion
			if (piece_ptr->colour == WHITE && new_rank == 7 || piece_ptr->colour == BLACK && new_rank == 0){
				add_pawn_capture_promotions(move_list_ptr, board_ptr, piece_ptr->square, target_square);
			}
			else {
				Move move = {};
				set_move(&move, piece_ptr->square, target_square, CAPTURE);
				move_list_ptr->moves[move_list_ptr->move_count++] = move;
			}
		}
	}  
	new_file = file - 1;
	if (inside_board(new_file, new_rank)) {
		target_square = coordinate_to_index(new_file, new_rank);
		target_piece_ptr = board_ptr->squares[target_square];
		if (target_piece_ptr && piece_ptr->colour != target_piece_ptr->colour) {
			// Check for pawn capture promotion
			if (piece_ptr->colour == WHITE && new_rank == 7 || piece_ptr->colour == BLACK && new_rank == 0){
				add_pawn_capture_promotions(move_list_ptr, board_ptr, piece_ptr->square, target_square);
			}
			else {
				Move move = {};
				set_move(&move, piece_ptr->square, target_square, CAPTURE);
				move_list_ptr->moves[move_list_ptr->move_count++] = move;
			}
		}
	}
}


void get_knight_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	get_set_moves(move_list_ptr, board_ptr, piece_ptr, knight_moves, 8);
}


void get_bishop_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	get_sliding_moves(move_list_ptr, board_ptr, piece_ptr, bishop_directions, 4);
}


void get_rook_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	get_sliding_moves(move_list_ptr, board_ptr, piece_ptr, rook_directions, 4);
}


void get_queen_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	get_sliding_moves(move_list_ptr, board_ptr, piece_ptr, bishop_directions, 4);
	get_sliding_moves(move_list_ptr, board_ptr, piece_ptr, rook_directions, 4);
}


void get_king_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	get_set_moves(move_list_ptr, board_ptr, piece_ptr, king_moves, 8);
}


void generate_piece_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	switch (piece_ptr->type) {
		case PAWN: 
			get_pawn_moves(move_list_ptr, board_ptr, piece_ptr);
			break;
		case KNIGHT:
			get_knight_moves(move_list_ptr, board_ptr, piece_ptr);
			break;
		case BISHOP:
			get_bishop_moves(move_list_ptr, board_ptr, piece_ptr);
			break;
		case ROOK:
			get_rook_moves(move_list_ptr, board_ptr, piece_ptr);
			break;
		case QUEEN:
			get_queen_moves(move_list_ptr, board_ptr, piece_ptr);
			break;
		case KING:
			get_king_moves(move_list_ptr, board_ptr, piece_ptr);
			break;
	}
}


void generate_pseudo_moves(MoveList* move_list_ptr, Board* board_ptr) {
	Piece* piece_ptr;
	for (int i = 0; i < 16; i++) {
		piece_ptr = &board_ptr->player_pieces[board_ptr->current_turn][i];
		if (!piece_ptr) {
			break;
		}
		if (!piece_ptr->alive) {
			continue;
		}
		generate_piece_moves(move_list_ptr, board_ptr, piece_ptr);
	}
}


bool is_legal(Move* move_ptr, Board* board_ptr) {
	bool legal = true;

	// Play move on board
	Piece* captured_piece_ptr = make_move(move_ptr, board_ptr);
	Colour king_colour = board_ptr->current_turn;
	switch_current_turn(board_ptr);
	
	// Generate pseudo-legal moves for opponent
	MoveList opponent_move_list = {};
	generate_pseudo_moves(&opponent_move_list, board_ptr);
	

	// Check if current player's king is under attack
	for (int i = 0; i < opponent_move_list.move_count; i++) {
		Square king_square = board_ptr->player_pieces[king_colour][0].square;
		if (opponent_move_list.moves[i].to == king_square) {
			legal = false;
			break;
		}
	}
	
	// Undo move on board
	undo_move(move_ptr, board_ptr, captured_piece_ptr);
	switch_current_turn(board_ptr);

	return legal;
}


void find_legal_moves(MoveList* move_list_ptr, Board* board_ptr) {
	int legal_moves = 0;
	for (int i = 0; i < move_list_ptr->move_count; i++) {
		if (is_legal(&move_list_ptr->moves[i], board_ptr)) {
			if (i != legal_moves) {
				move_list_ptr->moves[legal_moves] = move_list_ptr->moves[i];
			}
			legal_moves++;
		}
	}
	move_list_ptr->move_count = legal_moves;
}
