#include "chess.h"


Square position_to_index(int x, int y) {
	return 64 - (y * 8 + (8 - x));
}
