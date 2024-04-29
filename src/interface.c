#include <stdio.h>
#include "chess.h"


char piece_symbol_table[2][6] = {
	{'P', 'N', 'B', 'R', 'Q', 'K'},
	{'p', 'n', 'b', 'r', 'q', 'k'},
};


char piece_symbol(Piece* piece) {
	return piece_symbol_table[piece->colour][piece->type];
}


/* TODO: Add unicode board characters: ♟♞♝♜♛♚♙♘♗♖♕♔ */
void print_board(Board board) {
	printf("\n");
	for (int y = 0; y < 8; y++) {
		printf("%d   ", 8 - y);
		for (int x = 0; x < 8; x++) {
			int i = position_to_index(x, y);
			Piece* piece_ptr = board.squares[i];

			if (piece_ptr) { printf("%c ", piece_symbol(piece_ptr)); }
			else { printf(". "); }
		}
		printf("\n");
	}
	printf("\n    a b c d e f g h\n");
}
