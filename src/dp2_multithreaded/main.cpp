#include "board.h"

#include<iostream>

int main() {
	int rows, cols;
	std::cin >> rows >> cols;

	Board board(cols);

	for(int r = 1; r <= rows; ++r)
		std::cout << r << 'x' << cols << " -> " << board.GetSolution(r) << '\n';
}
