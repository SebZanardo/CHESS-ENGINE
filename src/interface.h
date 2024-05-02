#ifndef INTERFACE_H
#define INTERFACE_H


#include "chess.h"


/* FUNCTION DEFINITIONS */
void print_board(Board* board_ptr);
void print_board_details(Board* board_ptr);
void print_move_list(MoveList* move_list_ptr);
int get_move_index(MoveList* move_list_ptr);


#endif  /* INTERFACE_H */
