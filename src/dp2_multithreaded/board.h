#ifndef __BOARD_H
#define __BOARD_H

#include <unordered_map>
#include <vector>

class Board {
	private:
		const uint32_t COLS;
		const uint32_t MASK_SIZE;

		std::vector<std::unordered_map<uint32_t, int>> double_rows, rows_from, rows_to;

		std::vector<int> solutions;

		void computeDoubleRows(int, uint32_t, uint32_t, int);

		inline bool isRowFilled(uint32_t x, uint32_t y) {
			uint32_t z = x & y;
			return (x | y | (((z | MASK_SIZE) >> 1) & ((z << 1) | 1))) == MASK_SIZE - 1;
		}

	public:
		Board(uint32_t);

		int getSolution(uint32_t);
};

#endif // __BOARD_H
