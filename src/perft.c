#include <stdio.h>
#include <time.h>
#include "board.h"
#include "move_generation.h"


long long perft(Board* board_ptr, int depth) {
	MoveList move_list = {};
	generate_pseudo_moves(&move_list, board_ptr);
	find_legal_moves(&move_list, board_ptr);

	if (move_list.move_count == 0) {
		return 0;
	}

	if (depth == 0)
	{
		return 1;
	}

	if (depth == 1) 
	{
		return move_list.move_count;
	}

	long long nodes = 0;
	for (int i = 0; i < move_list.move_count; i++) {
		Move* selected_move_ptr = &move_list.moves[i];
		Piece* captured_piece_ptr = make_move(selected_move_ptr, board_ptr);

		// Save irreversible board data
		Square saved_en_passant_target = board_ptr->en_passant_target;
		bool saved_castling_rights[2][2] = {};
		saved_castling_rights[WHITE][KINGSIDE] = board_ptr->castling_rights[WHITE][KINGSIDE];
		saved_castling_rights[WHITE][QUEENSIDE] = board_ptr->castling_rights[WHITE][QUEENSIDE];
		saved_castling_rights[BLACK][KINGSIDE] = board_ptr->castling_rights[BLACK][KINGSIDE];
		saved_castling_rights[BLACK][QUEENSIDE] = board_ptr->castling_rights[BLACK][QUEENSIDE];

		update_en_passant_target(selected_move_ptr, board_ptr);
		update_castling_rights(selected_move_ptr, board_ptr);

		switch_current_turn(board_ptr);

		nodes += perft(board_ptr, depth - 1);

		switch_current_turn(board_ptr);
		undo_move(&move_list.moves[i], board_ptr, captured_piece_ptr);

		// Overwrite irreversible board data with saved data
		board_ptr->en_passant_target = saved_en_passant_target;
		board_ptr->castling_rights[WHITE][KINGSIDE] = saved_castling_rights[WHITE][KINGSIDE];
		board_ptr->castling_rights[WHITE][QUEENSIDE] = saved_castling_rights[WHITE][QUEENSIDE];
		board_ptr->castling_rights[BLACK][KINGSIDE] = saved_castling_rights[BLACK][KINGSIDE];
		board_ptr->castling_rights[BLACK][QUEENSIDE] = saved_castling_rights[BLACK][QUEENSIDE];
	}
	return nodes;
}


void run_perft_test(char* fen_string, long long* expected_results, int max_depth) {
	Board board = {};
	setup_board(&board, fen_string);

	printf("%s\n", fen_string);
	for (int i = 0; i < max_depth; i++) {
		// Benchmark time taken for the perft function
		float start_time = (float)clock()/CLOCKS_PER_SEC;

		long long found = perft(&board, i + 1);

		float end_time = (float)clock()/CLOCKS_PER_SEC;
		float time_elapsed = end_time - start_time;

		long long expected = expected_results[i];
		printf((found == expected) ? "\033[0;32mPASSED!\033[0m": "\033[31mFAILED!\033[0m");
		printf(" took: %.6fs\t", time_elapsed);
		printf("[depth:%d] ", i + 1);
		printf("found: %lld ", found);
		printf("expected: %lld\n", expected);
	}
	printf("\n");
}


// Positions and expected results from:
// https://www.chessprogramming.org/Perft_Results
void run_perft_suite() {
	run_perft_test(
		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
		(long long [8]) {20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956},
		4
	);

	run_perft_test(
		"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
		(long long [6]) {48, 2039, 97862, 4085603, 193690690, 8031647685},
		4
	);

	run_perft_test(
		"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
		(long long [8]) {14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393},
		4
	);

	run_perft_test(
		"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
		(long long [6]) {6, 264, 9467, 422333, 15833292, 706045033},
		4
	);

	run_perft_test(
		"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
		(long long [5]) {44, 1486, 62379, 2103487, 89941194},
		4
	);

	run_perft_test(
		"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
		(long long [6]) {46, 2079, 89890, 3894594, 164075551, 6923051137},
		4
	);
}
