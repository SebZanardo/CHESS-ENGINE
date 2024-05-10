#include <stdio.h>  // for getchar, printf and scanf
#include "chess.h"
#include "board.h"


char piece_symbol_table[2][6] = {
	{'P', 'N', 'B', 'R', 'Q', 'K'},
	{'p', 'n', 'b', 'r', 'q', 'k'},
};


char piece_symbol(Piece* piece_ptr) {
	return piece_symbol_table[piece_ptr->colour][piece_ptr->type];
}


/* TODO: Add unicode board characters: ♟♞♝♜♛♚♙♘♗♖♕♔ */
void print_board(Board* board_ptr) {
	printf("\n");
	for (int y = 0; y < 8; y++) {
		printf("%d   ", 8 - y);
		for (int x = 0; x < 8; x++) {
			int i = position_to_index(x, y);
			Piece* piece_ptr = board_ptr->squares[i];
			if (piece_ptr) { printf("%c ", piece_symbol(piece_ptr)); }
			else { printf(". "); }
		}
		printf("\n");
	}
	printf("\n    a b c d e f g h\n");
}


void print_board_details(Board* board_ptr) {
	printf("\n");
	switch(board_ptr->current_turn){
		case WHITE: printf("WHITE's turn\n"); break;
		case BLACK: printf("BLACK's turn\n"); break;
	}

	printf("WHITE castling rights:\n");
	printf("  KINGSIDE= %d\n", board_ptr->castling_rights[WHITE][KINGSIDE]);
	printf("  QUEENSIDE= %d\n", board_ptr->castling_rights[WHITE][QUEENSIDE]);
	printf("BLACK castling rights:\n");
	printf("  KINGSIDE= %d\n", board_ptr->castling_rights[BLACK][KINGSIDE]);
	printf("  QUEENSIDE= %d\n", board_ptr->castling_rights[BLACK][QUEENSIDE]);

	printf("EN PASSANT TARGET= %d\n", board_ptr->en_passant_target);

	printf("%d half moves\n", board_ptr->half_moves);
	printf("%d full moves\n", board_ptr->full_moves);
}


void print_move_list(MoveList* move_list_ptr) {
	printf("\n");
	for (int i = 0; i < move_list_ptr->move_count; i++) {
		printf("[%d] ", i);
		printf("from= %d, ", move_list_ptr->moves[i].from);
		printf("to= %d, ", move_list_ptr->moves[i].to);
		printf("type= %d\n", move_list_ptr->moves[i].type);
	}
}


int get_move_index(MoveList* move_list_ptr) {
	int i;
	// Trap in loop until user gives valid response
	while (1) {
		printf("Enter move index: ");
		scanf("%d", &i);
		while (getchar() != '\n');  // Clear the input buffer
		if (i >= 0 && i < move_list_ptr->move_count) {
			break;
		}
	}
	return i;
}
