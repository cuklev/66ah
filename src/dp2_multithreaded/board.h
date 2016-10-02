#ifndef __BOARD_H
#define __BOARD_H

#include <unordered_map>
#include <vector>
#include <array>

#include <cstdint>

#include <thread>
#include <atomic>

#define THREAD_COUNT 8

template<int COLS>
class Board {
	private:
		static constexpr uint32_t MASK_SIZE = 1u << COLS;
		using DoubeRow = std::array<std::unordered_map<uint32_t, int>, MASK_SIZE>;

		DoubeRow double_rows;
		DoubeRow rows_from, rows_to;

		std::vector<int> solutions;

		void initialize(int col, uint32_t first, uint32_t second, int pieces) {
			if(col == 0) {
				double_rows[second].insert({first, pieces});
				return;
			}

			initialize(col - 1, first, second, pieces);

			if(col == 1) return;

			first  |= 0b11u << (col - 2);
			second |= 0b01u << (col - 2);
			initialize(col - 2, first, second, pieces + 1);

			second ^= 0b11u << (col - 2);
			initialize(col - 2, first, second, pieces + 1);

			first  ^= 0b01u << (col - 2);
			second |= 0b01u << (col - 2);
			initialize(col - 2, first, second, pieces + 1);

			first  ^= 0b11u << (col - 2);
			initialize(col - 2, first, second, pieces + 1);

			if(col == 2) return;

			first  |= 0b111u << (col - 3);
			second |= 0b111u << (col - 3);
			initialize(col - 3, first, second, pieces + 2);
		}

		inline bool isRowFilled(uint32_t x, uint32_t y) {
			uint32_t z = x & y;
			return (x | y | (((z | MASK_SIZE) >> 1) & ((z << 1) | 1))) == MASK_SIZE - 1;
		}

	public:
		Board() {
			solutions = {0, 0};
			initialize(COLS, 0, 0, 0);
		}

		int getSolution(uint32_t rows) {
			if(rows < solutions.size())
				return solutions[rows];

			if(solutions.size() == 2)
				rows_from[0].insert({MASK_SIZE - 1, 0});

			for(uint32_t r = solutions.size(); r <= rows; ++r) {
				std::atomic<uint32_t> mask(0);
				std::atomic<int> result(1 << 30);

				std::vector<std::thread> threads;

				for(int i = 0; i < THREAD_COUNT; ++i)
					threads.emplace_back([&mask, &result, this]() {
						uint32_t middle;
						while(1) {
							middle = mask++;
							if(middle >= MASK_SIZE) break;

							for(uint32_t i = 0; i <= middle; ++i) {
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

				for(uint32_t i = 0; i < MASK_SIZE; ++i)
					rows_from[i].clear();
				for(uint32_t i = 0; i < MASK_SIZE; ++i) {
					for(auto&x : rows_to[i]) {
						rows_from[x.first].insert({i, x.second});
					}
					rows_to[i].clear();
				}

				solutions.push_back(result);
			}

			return solutions.back();
		}
};

#endif // __BOARD_H
