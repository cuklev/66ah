#include "board.h"

#include<iostream>

int main() {
	const int COLS = 12;
	Board<COLS> board;

	for(int i = 1; i < 15; ++i)
		std::cout << COLS << 'x' << i << " -> " << board.getSolution(i) << '\n';
}
