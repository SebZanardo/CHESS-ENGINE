#ifndef BOARD_H
#define BOARD_H


#include "chess.h"


/* FUNCTION DEFINITIONS */
bool inside_board(int file, int rank);
int index_to_file(Square square);
int index_to_rank(Square square);
Square position_to_index(int x, int y);
Square coordinate_to_index(int file, int rank);
void setup_board(Board* board_ptr, char* fen_string);
void switch_current_turn(Board* board_ptr);


#endif  /* BOARD_H */
