// gcc -o out main.c board.c chess.c interface.c move_generation.c perft.c
#include "chess.h"
#include "perft.h"


int main(void) {
	// play_game();
	run_perft_suite();
	return 0;
}
