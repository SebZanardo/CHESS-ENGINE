#ifndef CHESS_H
#define CHESS_H


#include <stdbool.h>  // for bool


typedef enum {
	WHITE,
	BLACK,
} Colour;


/* Value = Index of square in Board.squares */
typedef enum {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	NONE
} Square;


typedef enum {
	QUEENSIDE,
	KINGSIDE,
} CastleSide;


typedef enum {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
} PieceType;


typedef enum {
	QUIET_MOVE,
	CAPTURE,
	CASTLE_KINGSIDE,
	CASTLE_QUEENSIDE,
	DOUBLE_PAWN_PUSH,
	EN_PASSANT,
	PROMOTION_KNIGHT,
	PROMOTION_BISHOP,
	PROMOTION_ROOK,
	PROMOTION_QUEEN,
	CAPTURE_PROMOTION_KNIGHT,
	CAPTURE_PROMOTION_BISHOP,
	CAPTURE_PROMOTION_ROOK,
	CAPTURE_PROMOTION_QUEEN,
} MoveType;


typedef struct {
	Square from;
	Square to;
	MoveType type;
} Move;


typedef struct {
	Move moves[218];  // 218 is the maximum number of moves in a chess position
	int move_count;
} MoveList;


typedef struct {
	PieceType type;
	Colour colour;
	Square square;
	bool alive;
} Piece;


/* TODO: Alter to handle any number of players */
typedef struct {
	Piece* squares[64];
	Piece player_pieces[2][16];

	Colour current_turn;
	int half_moves;
	int full_moves;

	Square en_passant_target;
	bool castling_rights[2][2];
} Board;


/* FUNCTION DEFINITIONS */
Square position_to_index(int x, int y);
void setup_board(Board* board_ptr, char* fen_string);
void generate_pseudo_moves(MoveList* move_list_ptr, Board* board_ptr);
void switch_current_turn(Board* board_ptr);
Piece* make_move(Move* move_ptr, Board* board_ptr);
void find_legal_moves(MoveList* move_list_ptr, Board* board_ptr);


#endif  /* CHESS_H */
