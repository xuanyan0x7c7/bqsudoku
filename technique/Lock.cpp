#include <algorithm>
#include <string>
#include <sstream>
#include "Lock.h"
using std::make_pair;
using std::pair;
using std::ostringstream;
using std::reverse;
using std::size_t;
using std::string;
using std::vector;


Lock::Lock(const Lock&) = default;
Lock::Lock(Lock&&) = default;
Lock& Lock::operator =(const Lock&) = delete;
Lock& Lock::operator =(Lock&&) = delete;
Lock::~Lock() = default;

Lock::Lock(Candidate &sudoku): Technique(sudoku) {}

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

Technique::HintType Lock::GetHint() {
	HintType hint;
	hint = LockedCandidates();
	if (!hint.first.empty()) {
		return hint;
	}
	for (size_t pair_size = 2; pair_size <= size / 2; ++pair_size) {
		hint = HiddenPair(pair_size);
		if (!hint.first.empty()) {
			return hint;
		}
		hint = NakedPair(pair_size);
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
}

Technique::HintType Lock::LockedCandidates() {
	for (size_t row = 0; row < size; ++row) {
		for (size_t number = 1; number <= size; ++number) {
			vector<size_t> count(m);
			size_t sum = 0;
			size_t column;
			for (size_t i = 0; i < size; ++i) {
				if ((*this)(row, i, number)) {
					++count[i / n];
					++sum;
					column = i;
				}
			}
			if (sum == 0) {
				continue;
			}
			vector<pair<size_t,size_t>> elim;
			for (size_t i = 0; i < m; ++i) if (count[i] == sum) {
				for (size_t r = 0; r < m; ++r) if (r != row % m) {
					for (size_t c = 0; c < n; ++c) {
						if ((*this)(row / m * m + r, column / n * n + c, number)) {
							elim.push_back(make_pair(row / m * m + r, column / n * n + c));
						}
					}
				}
				break;
			}
			if (!elim.empty()) {
				difficulty += 100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number) << " in Row " << Row2Char(row) << "):";
				for (auto &x: elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second) << "!=" << Number2Char(number);
				}
				return make_pair(ostr.str(), false);
			}
		}
	}

	for (size_t column = 0; column < size; ++column) {
		for (size_t number = 1; number <= size; ++number) {
			vector<size_t> count(n);
			size_t sum = 0;
			size_t row;
			for (size_t i = 0; i < size; ++i) {
				if ((*this)(i, column, number)) {
					++count[i / m];
					++sum;
					row = i;
				}
			}
			if (sum == 0) {
				continue;
			}
			vector<pair<size_t,size_t>> elim;
			for (size_t i = 0; i < n; ++i) if (count[i] == sum) {
				for (size_t c = 0; c < n; ++c) if (c != column % n) {
					for (size_t r = 0; r < m; ++r) {
						if ((*this)(row / m * m + r, column / n * n + c, number)) {
							elim.push_back(make_pair(row / m * m + r, column / n * n + c));
						}
					}
				}
				break;
			}
			if (!elim.empty()) {
				difficulty += 100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number)
					<< " in Column " << Column2Char(column) << "):";
				for (auto &x: elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second) << "!=" << Number2Char(number);
				}
				return make_pair(ostr.str(), false);
			}
		}
	}

	for (size_t box = 0; box < size; ++box) {
		for (size_t number = 1; number <= size; ++number) {
			vector<size_t> count_row(m), count_column(n);
			size_t sum = 0;
			size_t row, column;
			for (size_t i = 0; i < m; ++i) for (size_t j = 0; j < n; ++j) {
				if ((*this)(box / m * m + i, box % m * n + j, number)) {
					++count_row[i];
					++count_column[j];
					++sum;
					row = box / m * m + i;
					column = box % m * n + j;
				}
			}
			if (sum == 0) {
				continue;
			}
			vector<pair<size_t,size_t>> row_elim, column_elim;
			for (size_t i = 0; i < m; ++i) if (count_row[i] == sum) {
				for (size_t c = 0; c < size; ++c) {
					if (c / n != box % m) {
						if ((*this)(row ,c, number)) {
							row_elim.push_back(make_pair(row, c));
						}
					}
				}
				break;
			}
			for (size_t i = 0; i < n; ++i) if (count_column[i] == sum) {
				for (size_t r = 0; r < size; ++r) {
					if (r / m != box / m) {
						if ((*this)(r, column, number)) {
							column_elim.push_back(make_pair(r, column));
						}
					}
				}
				break;
			}
			if (!row_elim.empty()) {
				difficulty += 100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number) << " in Box " << box + 1 << "):";
				for (auto &x: row_elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second) << "!=" << Number2Char(number);
				}
				return make_pair(ostr.str(), false);
			}
			if (!column_elim.empty()) {
				difficulty += 0x100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number) << " in Box " << box + 1 << "):";
				for (auto &x: column_elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second) << "!=" << Number2Char(number);
				}
				return make_pair(ostr.str(), false);
			}
		}
	}

	return make_pair("", false);
}

namespace {
	static const string pair_name[11] = {"", "", "Pair", "Triple", "Quad",
		"Pent", "Hex", "Sept", "Oct", "Nov", "Dec"};
}

Technique::HintType Lock::NakedPair(size_t pair_size) {
	string name = pair_size < 11 ? pair_name[pair_size]
		: string("Pair") + Num2Str(pair_size);
	for (size_t box = 0; box < size; ++box) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (box_count[box][number - 1] > 0) {
				num_available.push_back(number);
			}
		}
		size_t blank_size = num_available.size();
		if (pair_size > blank_size / 2) {
			continue;
		}

		size_t r = box / m * m;
		size_t c = box % m * n;
		vector<bool> set(blank_size);
		for (size_t i = 0; i < pair_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> contain, not_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				(set[i] ? contain : not_contain).push_back(num_available[i]);
			}
			vector<bool> g(size, true);
			size_t count = blank_size;
			for (size_t i = 0; i < m; ++i) for (size_t j = 0; j < n; ++j) {
				if (board[(r + i) * size + (c + j)] == 0) {
					for (size_t number: not_contain) {
						if ((*this)(r + i, c + j, number)) {
							--count;
							g[i * n + j] = false;
							break;
						}
					}
				}
			}
			if (count == pair_size) {
				vector<size_t> elim;
				for (size_t i = 0; i < size; ++i) if (!g[i]) {
					for (size_t number: contain) {
						if ((*this)(r + i / n, c + i % n, number)) {
							elim.push_back((r + i / n) * size + (c + i % n));
							break;
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 100 + 20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Naked " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Box " << box + 1 << "):";
					for (size_t cell: elim) {
						ostr << ' ' << Row2Char(cell / size) << Column2Char(cell % size) << "!=";
						for (size_t number: contain) {
							if ((*this)(cell, number)) {
								Remove(cell, number);
								ostr << Number2Char(number);
							}
						}
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	for (size_t row = 0; row < size; ++row) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (row_count[row][number - 1] > 0) {
				num_available.push_back(number);
			}
		}
		size_t blank_size = num_available.size();
		if (pair_size > blank_size / 2) {
			continue;
		}

		vector<bool> set(blank_size);
		for (size_t i = 0; i < pair_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> contain, not_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				(set[i] ? contain : not_contain).push_back(num_available[i]);
			}
			vector<bool> column(size, true);
			size_t count = blank_size;
			for (size_t i = 0; i < size; ++i) if (board[row * size + i] == 0) {
				for (size_t number: not_contain) {
					if ((*this)(row, i, number)) {
						--count;
						column[i] = false;
						break;
					}
				}
			}
			if (count == pair_size) {
				vector<size_t> elim;
				for (size_t i = 0; i < size; ++i) if (!column[i]) {
					for (size_t number: contain) {
						if ((*this)(row, i, number)) {
							elim.push_back(i);
							break;
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 100 + 20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Naked " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Row " << Row2Char(row) << "):";
					for (size_t column: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column) << "!=";
						for (size_t number: contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	for (size_t column = 0; column < size; ++column) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (column_count[column][number - 1] > 0) {
				num_available.push_back(number);
			}
		}
		size_t blank_size = num_available.size();
		if (pair_size > blank_size / 2) {
			continue;
		}

		vector<bool> set(blank_size);
		for (size_t i = 0; i < pair_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> contain, not_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				(set[i] ? contain : not_contain).push_back(num_available[i]);
			}
			vector<bool> row(size, true);
			size_t count = blank_size;
			for (size_t i = 0; i < size; ++i) if (board[i * size + column] == 0) {
				for (size_t number: not_contain) {
					if ((*this)(i, column, number)) {
						--count;
						row[i] = false;
						break;
					}
				}
			}
			if (count == pair_size) {
				vector<size_t> elim;
				for (size_t i = 0; i < size; ++i) if (!row[i]) {
					for (size_t number: contain) {
						if ((*this)(i, column, number)) {
							elim.push_back(i);
							break;
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 100 + 20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Naked " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Column " << Column2Char(column) << "):";
					for (size_t row: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column) << "!=";
						for (size_t number: contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	return make_pair("", false);
}

Technique::HintType Lock::HiddenPair(size_t pair_size) {
	string name = pair_size < 11 ? pair_name[pair_size]
		: string("Pair") + Num2Str(pair_size);

	for (size_t box = 0; box < size; ++box) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (box_count[box][number - 1] > 0) {
				num_available.push_back(number);
			}
		}
		size_t blank_size = num_available.size();
		if (pair_size > blank_size / 2) {
			continue;
		}

		size_t r = box / m * m;
		size_t c = box % m * n;
		vector<bool> set(blank_size);
		for (size_t i = 0; i < pair_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> contain, not_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				(set[i] ? contain : not_contain).push_back(num_available[i]);
			}
			vector<size_t> g;
			for (size_t i = 0; i < m; ++i) for (size_t j = 0; j < n; ++j) {
				for (size_t number: contain) {
					if ((*this)(r + i, c + j, number)) {
						g.push_back(i * n + j);
						break;
					}
				}
			}
			if (g.size() == pair_size) {
				vector<size_t> elim;
				for (size_t i: g) {
					for (size_t number: not_contain) {
						if ((*this)(r + i / n, c + i % n, number)) {
							elim.push_back(i);
							break;
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 100 + 20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Hidden " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Box " << box + 1 << "):";
					for (size_t k: elim) {
						size_t row = r + k / n;
						size_t column = c + k % n;
						ostr << ' ' << Row2Char(row) << Column2Char(column) << "!=";
						for (size_t number: not_contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	for (size_t row = 0; row < size; ++row) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (row_count[row][number - 1] > 0) {
				num_available.push_back(number);
			}
		}
		size_t blank_size = num_available.size();
		if (pair_size > blank_size / 2) {
			continue;
		}

		vector<bool> set(blank_size);
		for (size_t i = 0; i < pair_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> contain, not_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				(set[i] ? contain : not_contain).push_back(num_available[i]);
			}
			vector<size_t> column;
			for (size_t i = 0; i < size; ++i) {
				for (size_t number: contain) {
					if ((*this)(row, i, number)) {
						column.push_back(i);
						break;
					}
				}
			}
			if (column.size() == pair_size) {
				vector<size_t> elim;
				for (size_t c: column) {
					for (size_t number: not_contain) {
						if ((*this)(row, c, number)) {
							elim.push_back(c);
							break;
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 100 + 20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Hidden " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Row " << Row2Char(row) << "):";
					for (size_t column: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column) << "!=";
						for (size_t number: not_contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	for (size_t column = 0; column < size; ++column) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (column_count[column][number - 1] > 0) {
				num_available.push_back(number);
			}
		}
		size_t blank_size = num_available.size();
		if (pair_size > blank_size / 2) {
			continue;
		}

		vector<bool> set(blank_size);
		for (size_t i = 0; i < pair_size; ++i) {
			set[i] = true;
		}
		do {
			vector<size_t> contain, not_contain;
			for (size_t i = 0; i < blank_size; ++i) {
				(set[i] ? contain : not_contain).push_back(num_available[i]);
			}
			vector<size_t> row;
			for (size_t i = 0; i < size; ++i) {
				for (size_t number: contain) {
					if ((*this)(i, column, number)) {
						row.push_back(i);
						break;
					}
				}
			}
			if (row.size() == pair_size) {
				vector<size_t> elim;
				for (size_t r: row) {
					for (size_t number: not_contain) {
						if ((*this)(r, column, number)) {
							elim.push_back(r);
							break;
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 100 + 20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Hidden " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Column " << Column2Char(column) << "):";
					for (size_t row: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column) << "!=";
						for (size_t number: not_contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return make_pair(ostr.str(), false);
				}
			}
		} while (NextCombination(set));
	}

	return make_pair("", false);
}
