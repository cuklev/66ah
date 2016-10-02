#include "board.h"

#include <iostream>

int main() {
	const int ROWS = 9;
	const int COLS = 9;
	Board board(COLS);
	std::cout << board.getSolution(ROWS) << '\n';
}
