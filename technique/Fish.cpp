#include <algorithm>
#include <string>
#include <sstream>
#include "Fish.h"
using std::make_pair;
using std::ostringstream;
using std::reverse;
using std::size_t;
using std::string;
using std::vector;


namespace {
	bool NextCombination(vector<bool> &vec) {
		size_t size = vec.size();
		for (size_t pos = 0; pos < size - 1; ++pos) {
			if (vec[pos] && !vec[pos + 1]) {
				vec[pos] = false;
				vec[pos + 1] = true;
				reverse(vec.begin(), vec.begin() + pos);
				return true;
			}
		}

		reverse(vec.begin(), vec.end());
		return false;
	}

	inline string Num2Str(size_t num) {
		ostringstream ostr;
		ostr << num;
		return ostr.str();
	}
}

Fish::Fish(const Fish&) = default;
Fish::Fish(Fish&&) = default;
Fish& Fish::operator =(const Fish&) = delete;
Fish& Fish::operator =(Fish&&) = delete;
Fish::~Fish() = default;

Fish::Fish(Candidate &sudoku): Technique(sudoku) {}

Technique::HintType Fish::GetHint() {
	HintType hint;
	for (size_t fish_size = 2; fish_size <= size / 2; ++fish_size) {
		hint = _Fish(fish_size);
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
}

Technique::HintType Fish::_Fish(size_t fish_size) {
	static const string xwing_name[8] = {"", "", "X-Wing", "Swordfish", "Jellyfish", "Squirmbag", "Whale", "Leviathan"};
	string name = fish_size < 8 ? xwing_name[fish_size] : Num2Str(fish_size) + string("X-Wing");
	for (size_t number = 1; number <= size; ++number) {
		vector<size_t> row_available, column_available;
		for (size_t i = 0; i < size; ++i) {
			if (row_count[i][number - 1] > 0) {
				row_available.push_back(i);
			}
			if (column_count[i][number - 1] > 0) {
				column_available.push_back(i);
			}
		}
		if (fish_size > row_available.size() / 2) {
			continue;
		}
		size_t blank_size = row_available.size();

		vector<bool> set(blank_size);
		for (size_t i = 0; i < fish_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> row_contain, column_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				if (set[i]) {
					row_contain.push_back(row_available[i]);
					column_contain.push_back(column_available[i]);
				}
			}

			vector<bool> contain(size, false);
			for (size_t row: row_contain) {
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						contain[column] = true;
					}
				}
			}
			size_t count = 0;
			for (bool x: contain) if (x) {
				++count;
			}
			if (count == fish_size) {
				vector<size_t> elim;
				for (size_t column = 0; column < size; ++column) if (contain[column]) {
					for (size_t i = 0; i < blank_size; ++i) if (!set[i]) {
						if ((*this)(row_available[i], column, number)) {
							elim.push_back(row_available[i] * size + column);
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 1000 + 100 * (fish_size - 2);
					ostringstream ostr;
					ostr << name << " (" << Number2String(number) << " in Row ";
					for (size_t row: row_contain) {
						ostr << Row2String(row);
					}
					ostr << "):";
					for (size_t cell: elim) {
						Remove(cell, number);
						ostr << ' ' << Cell2String(cell) << "!=" << Number2String(number);
					}
					return make_pair(ostr.str(), false);
				}
			}

			contain.assign(size, false);
			for (size_t column: column_contain) {
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						contain[row] = true;
					}
				}
			}
			count = 0;
			for (bool x: contain) if (x) {
				++count;
			}
			if (count == fish_size) {
				vector<size_t> elim;
				for (size_t row = 0; row < size; ++row) if (contain[row]) {
					for (size_t i = 0; i < blank_size; ++i) if (!set[i]) {
						if ((*this)(row, column_available[i], number)) {
							elim.push_back(row * size + column_available[i]);
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 1000 + 100 * (fish_size - 2);
					ostringstream ostr;
					ostr << name << " (" << Number2String(number) << " in Column ";
					for (size_t column: column_contain) {
						ostr << Column2String(column);
					}
					ostr << "):";
					for (size_t cell: elim) {
						Remove(cell, number);
						ostr << ' ' << Cell2String(cell) << "!=" << Number2String(number);
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	return make_pair("", false);
}
