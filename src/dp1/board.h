#ifndef __BOARD_H
#define __BOARD_H

#include <map>
#include <vector>

class Board {
	private:
		uint32_t cols;

		using DoubleRow = std::pair<uint32_t, uint32_t>;

		std::map<DoubleRow, int> double_rows;
		std::map<DoubleRow, int> row_from, row_to;

		void initialize(uint32_t, DoubleRow, int);

		std::vector<int> solutions;

	public:
		Board(uint32_t);
		int getSolution(uint32_t);
};

#endif // __BOARD_H
