#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H


#include "chess.h"


/* FUNCTION DEFINITIONS */
void generate_pseudo_moves(MoveList* move_list_ptr, Board* board_ptr);
void find_legal_moves(MoveList* move_list_ptr, Board* board_ptr);


#endif  /* MOVE_GENERATION_H */
