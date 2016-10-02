#include "board.h"

void Board::initialize(uint32_t cols, DoubleRow drow, int pieces) {
	if(cols == 0) {
		double_rows.insert({drow, pieces});
		return;
	}

	initialize(cols - 1, drow, pieces);

	if(cols == 1) return;

	drow.first  |= 0b11u << (cols - 2);
	drow.second |= 0b01u << (cols - 2);
	initialize(cols - 2, drow, pieces + 1);

	drow.second ^= 0b11u << (cols - 2);
	initialize(cols - 2, drow, pieces + 1);

	drow.first  ^= 0b01u << (cols - 2);
	drow.second |= 0b11u << (cols - 2);
	initialize(cols - 2, drow, pieces + 1);

	drow.first  ^= 0b11u << (cols - 2);
	initialize(cols - 2, drow, pieces + 1);

	if(cols == 2) return;

	drow.first  |= 0b111u << (cols - 3);
	drow.second |= 0b111u << (cols - 3);
	initialize(cols - 3, drow, pieces + 2);
}

Board::Board(uint32_t cols) : cols(cols) {
	initialize(cols, {0, 0}, 0);
	solutions = {0, 0};
	row_from = {{{0, 0}, 0}};
}

inline bool isRowFilled(uint32_t cols, uint32_t left, uint32_t right) {
	auto x = left & right;
	return (left | right | (((x | (1u << cols)) >> 1) & ((x << 1) | 1))) + 1 == (1 << cols);
}

int Board::getSolution(uint32_t rows) {
	if(rows < solutions.size())
		return solutions[rows];

	if(solutions.size() == 2) {
		row_from = double_rows;
		int result = 1 << 30;
		for(auto& x : row_from) {
			if(result > x.second && isRowFilled(cols, x.first.first, x.first.second))
				result = x.second;
		}
		solutions.push_back(result);
	}

	for(uint32_t i = solutions.size(); i <= rows; ++i) {
		for(auto& x : row_from) {
			for(auto& y : double_rows) {
				if(x.first.second & y.first.first) continue;
				if(!isRowFilled(cols, x.first.first, x.first.second | y.first.first)) continue;
				auto p = row_to.insert({{x.first.second | y.first.first, y.first.second}, x.second + y.second});
				if(!p.second && p.first->second > x.second + y.second)
					p.first->second = x.second + y.second;
			}
		}

		row_from = std::move(row_to);

		int result = 1 << 30;
		for(auto& x : row_from) {
			if(result > x.second && isRowFilled(cols, x.first.first, x.first.second))
				result = x.second;
		}
		solutions.push_back(result);
	}

	return solutions.back();
}
