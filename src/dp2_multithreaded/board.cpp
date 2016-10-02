#include "board.h"

#include <thread>
#include <atomic>

#define THREAD_COUNT 8

void Board::initialize(int col, uint32_t x, uint32_t y, int pieces) {
	if(col == 0) {
		double_rows[x].insert({y, pieces});
		return;
	}

	--col;
	initialize(col, x, y, pieces);

	if(col == 0) return;

	--col; ++pieces;
	x |= 0b11u << col;
	y |= 0b01u << col;
	initialize(col, x, y, pieces);

	y ^= 0b11u << col;
	initialize(col, x, y, pieces);

	x  ^= 0b01u << col;
	y |= 0b01u << col;
	initialize(col, x, y, pieces);

	x  ^= 0b11u << col;
	initialize(col, x, y, pieces);

	if(col == 0) return;

	--col; ++pieces;
	x |= 0b111u << col;
	y |= 0b111u << col;
	initialize(col, x, y, pieces);
}

Board::Board(uint32_t cols) : COLS(cols), MASK_SIZE(1u << COLS) {
	solutions = {0, 0};
	double_rows.resize(MASK_SIZE);
	initialize(COLS, 0, 0, 0);
}

int Board::getSolution(uint32_t rows) {
	if(rows < solutions.size())
		return solutions[rows];

	if(solutions.size() == 2)
	{
		rows_from.resize(MASK_SIZE);
		rows_from[0].insert({MASK_SIZE - 1, 0});
		rows_to.resize(MASK_SIZE);
	}

	for(uint32_t r = solutions.size(); r <= rows; ++r) {
		std::atomic<uint32_t> mask(0);
		std::atomic<int> result(1 << 30);

		std::vector<std::thread> threads;

		for(int i = 0; i < THREAD_COUNT; ++i)
			threads.emplace_back([&mask, &result, this]() {
				while(1) {
					uint32_t middle = mask++;
					if(middle >= MASK_SIZE) break;

					for(uint32_t i = 0; i <= mask; ++i) {
						uint32_t j = middle ^ i;
						if(i & j) continue;

						for(auto &x : rows_from[i]) {
							if(!isRowFilled(x.first, middle)) continue;
							for(auto& y : double_rows[j]) {
								auto p = rows_to[middle].insert({y.first, x.second + y.second});
								if(!p.second && p.first->second > x.second + y.second)
									p.first->second = x.second + y.second;
								if(result > p.first->second && isRowFilled(middle, y.first))
									result = p.first->second;
							}
						}
					}
				}
			});

		for(auto& t : threads) t.join();

		for(auto& x : rows_from) x.clear();
		for(uint32_t i = 0; i < MASK_SIZE; ++i) {
			for(auto& x : rows_to[i]) {
				rows_from[x.first].insert({i, x.second});
			}
			rows_to[i].clear();
		}

		solutions.push_back(result);
	}

	return solutions.back();
}
