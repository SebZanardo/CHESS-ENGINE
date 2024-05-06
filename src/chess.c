#include "chess.h"
#include <stdbool.h>  // for bool
#include <stdlib.h>  // for atoi function


// pawn needs custom logic for each move
int knight_moves[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, -1}, {-2, 1}};
int king_moves[8][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int bishop_directions[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int rook_directions[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
// queen directions is rook_directions + bishop_directions


bool inside_board(int file, int rank) {
	return file >= 0 && file < 8 && rank >= 0 && rank < 8;
}


int index_to_file(Square square) {
	return square % 8;
}


int index_to_rank(Square square) {
	return (square - (square % 8)) / 8;
}


Square position_to_index(int x, int y) {
	return 64 - (y * 8 + (8 - x));
}


Square coordinate_to_index(int file, int rank) {
	return rank * 8 + file;
}


void set_piece(Piece* piece_ptr, PieceType type, Colour colour, Square square, bool alive) {
	piece_ptr->type = type;
	piece_ptr->colour = colour;
	piece_ptr->square = square;
	piece_ptr->alive = alive;
}


void set_move(Move* move_ptr, Square from, Square to, MoveType type) {
	move_ptr->from = from;
	move_ptr->to = to;
	move_ptr->type = type;
}


void symbol_to_piece_init(Piece* piece_ptr, char symbol, Square square) {
	switch (symbol) {
		case 'P': set_piece(piece_ptr, PAWN, WHITE, square, true); break;
		case 'N': set_piece(piece_ptr, KNIGHT, WHITE, square, true); break;
		case 'B': set_piece(piece_ptr, BISHOP, WHITE, square, true); break;
		case 'R': set_piece(piece_ptr, ROOK, WHITE, square, true); break;
		case 'Q': set_piece(piece_ptr, QUEEN, WHITE, square, true); break;
		case 'K': set_piece(piece_ptr, KING, WHITE, square, true); break;
		case 'p': set_piece(piece_ptr, PAWN, BLACK, square, true); break;
		case 'n': set_piece(piece_ptr, KNIGHT, BLACK, square, true); break;
		case 'b': set_piece(piece_ptr, BISHOP, BLACK, square, true); break;
		case 'r': set_piece(piece_ptr, ROOK, BLACK, square, true); break;
		case 'q': set_piece(piece_ptr, QUEEN, BLACK, square, true); break;
		case 'k': set_piece(piece_ptr, KING, BLACK, square, true); break;
	}
}


/* TODO: Add error handling for invalid FEN strings */
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
void setup_board(Board* board_ptr, char* fen_string) {
	// To keep track of which index of player_pieces to write the next piece to
	int piece_len[2] = {0};

	int y = 0;
	int x = 0;
	
	int i, c;
	// Loop through board fen_string characters and setup board
	for (i = 0; (c = fen_string[i]) != ' '; i++) {
		if (c == '/') {
			y++;
			x = 0;
		}
		else if (c > '0' && c < '9') {
			x += c - '0';
		}
		else {
			Piece piece = {};
			Piece* piece_ptr;
			int square = position_to_index(x, y);
			symbol_to_piece_init(&piece, c, square);

			if (piece.type == KING) {
				// Copy struct to player_pieces array at first index
				board_ptr->player_pieces[piece.colour][0] = piece;
				piece_ptr = &board_ptr->player_pieces[piece.colour][0];
			}
			else {
				// Copy piece struct to player_pieces array at next free index
				int next = ++piece_len[piece.colour];
				board_ptr->player_pieces[piece.colour][next] = piece;
				piece_ptr = &board_ptr->player_pieces[piece.colour][next];
			}
			// Board.squares points to copied piece in player_pieces array
			board_ptr->squares[square] = piece_ptr;
			x++;
		}
	}

	// Read and setup current turn from fen string
	c = fen_string[++i];
	board_ptr->current_turn = WHITE;
	if (c == 'b') { board_ptr->current_turn = BLACK; }

	// Read and setup castling rights from fen string
	board_ptr->castling_rights[WHITE][KINGSIDE] = false;
	board_ptr->castling_rights[WHITE][QUEENSIDE] = false;
	board_ptr->castling_rights[BLACK][QUEENSIDE] = false;
	board_ptr->castling_rights[BLACK][QUEENSIDE] = false;
	for (i += 2; (c = fen_string[i]) != ' '; i++) {
		switch (c) {
			case 'K': board_ptr->castling_rights[WHITE][KINGSIDE] = true; break;
			case 'Q': board_ptr->castling_rights[WHITE][QUEENSIDE] = true; break;
			case 'k': board_ptr->castling_rights[BLACK][KINGSIDE] = true; break;
			case 'q': board_ptr->castling_rights[BLACK][QUEENSIDE] = true; break;
		}
	}

	// Read and setup en passant target from fen string
	c = fen_string[++i];
	board_ptr->en_passant_target = NONE;
	if (c != '-') {
		int file = c - 'a';
		c = fen_string[++i];
		int rank = c - '1';
		board_ptr->en_passant_target = coordinate_to_index(file, rank);
	}

	// Read and setup half move and full moves from fen string
	i += 2;
	int digits;
	for (digits = 0; (c = fen_string[i+digits]) != ' '; digits++) {}
	char half_moves[digits];
	for (int j = 0; j < digits; j++) { half_moves[j] = fen_string[i + j]; }
	board_ptr->half_moves = atoi(half_moves);

	i += digits + 1;
	for (digits = 0; (c = fen_string[i+digits]); digits++) {}
	char full_moves[digits];
	for (int j = 0; j < digits; j++) { full_moves[j] = fen_string[i + j]; }
	board_ptr->full_moves = atoi(full_moves);
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

		Move move = {};
		if (!target_piece_ptr) {
			set_move(&move, piece_ptr->square, target_square, QUIET_MOVE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
		}
		else if (piece_ptr->colour != target_piece_ptr->colour) {
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

			Move move = {};
			if (!target_piece_ptr) {
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


void get_pawn_moves(MoveList* move_list_ptr, Board* board_ptr, Piece* piece_ptr) {
	int forward = piece_ptr->colour == WHITE ? 1 : -1;

	int file = index_to_file(piece_ptr->square);
	int rank = index_to_rank(piece_ptr->square);

	int new_file, new_rank;

	// Don't need to check inside board for pawn moves (still have to for captures) as if at the end then it has to promote to another piece
	// Move forward one
	new_file = file;
	new_rank = rank + forward;

	// Temporary whilst promotion not implemented
	if (!inside_board(new_file, new_rank)) { return; }  

	int target_square = coordinate_to_index(new_file, new_rank);
	Piece* target_piece_ptr = board_ptr->squares[target_square];
	if (!target_piece_ptr) {
		Move move = {};
		set_move(&move, piece_ptr->square, target_square, QUIET_MOVE);
		move_list_ptr->moves[move_list_ptr->move_count++] = move;
		
		// Check double pawn push
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

	// Captures
	new_rank = rank + forward;

	new_file = file + 1;
	if (inside_board(new_file, new_rank)) {
		target_square = coordinate_to_index(new_file, new_rank);
		target_piece_ptr = board_ptr->squares[target_square];
		if (target_piece_ptr && piece_ptr->colour != target_piece_ptr->colour) {
			Move move = {};
			set_move(&move, piece_ptr->square, target_square, CAPTURE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
		}
	}  
	new_file = file - 1;
	if (inside_board(new_file, new_rank)) {
		target_square = coordinate_to_index(new_file, new_rank);
		target_piece_ptr = board_ptr->squares[target_square];
		if (target_piece_ptr && piece_ptr->colour != target_piece_ptr->colour) {
			Move move = {};
			set_move(&move, piece_ptr->square, target_square, CAPTURE);
			move_list_ptr->moves[move_list_ptr->move_count++] = move;
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
		case PAWN: get_pawn_moves(move_list_ptr, board_ptr, piece_ptr); break;
		case KNIGHT: get_knight_moves(move_list_ptr, board_ptr, piece_ptr); break;
		case BISHOP: get_bishop_moves(move_list_ptr, board_ptr, piece_ptr); break;
		case ROOK: get_rook_moves(move_list_ptr, board_ptr, piece_ptr); break;
		case QUEEN: get_queen_moves(move_list_ptr, board_ptr, piece_ptr); break;
		case KING: get_king_moves(move_list_ptr, board_ptr, piece_ptr); break;
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


Colour get_opponent_colour(Colour player_colour) {
	if (player_colour == WHITE) {
		return BLACK;
	}
	return WHITE;
}


void switch_current_turn(Board* board_ptr) {
	board_ptr->current_turn = get_opponent_colour(board_ptr->current_turn);
}


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


bool is_legal(Move* move_ptr, Board* board_ptr) {
	bool legal = true;

	// Play move on board
	Piece* captured_piece_ptr = make_move(move_ptr, board_ptr);
	switch_current_turn(board_ptr);
	
	// Generate pseudo-legal moves for opponent
	MoveList opponent_move_list = {};
	generate_pseudo_moves(&opponent_move_list, board_ptr);
	
	switch_current_turn(board_ptr);

	// Check if current player's king is under attack
	for (int i = 0; i < opponent_move_list.move_count; i++) {
		Square king_square = board_ptr->player_pieces[board_ptr->current_turn][0].square;
		if (opponent_move_list.moves[i].to == king_square) {
			legal = false;
			break;
		}
	}
	
	// Undo move on board
	undo_move(move_ptr, board_ptr, captured_piece_ptr);

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
