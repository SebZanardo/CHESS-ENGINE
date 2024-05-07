#include <stdbool.h>  // for bool
#include <stdlib.h>  // for atoi function
#include "chess.h"


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


Colour get_opponent_colour(Colour player_colour) {
	if (player_colour == WHITE) {
		return BLACK;
	}
	return WHITE;
}


void switch_current_turn(Board* board_ptr) {
	board_ptr->current_turn = get_opponent_colour(board_ptr->current_turn);
}
