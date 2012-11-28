#include <algorithm>
#include <array>
#include <deque>
#include <list>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>
#include "Candidate.h"
using namespace BQSudoku;
using std::array;
using std::deque;
using std::list;
using std::make_pair;
using std::ostringstream;
using std::pair;
using std::reverse;
using std::set;
using std::size_t;
using std::sort;
using std::string;
using std::unordered_map;
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

	string Num2Str(size_t num) {
		ostringstream ostr;
		ostr << num;
		return ostr.str();
	}
}

Candidate::Candidate(const Candidate&) = delete;
Candidate::Candidate(Candidate&&) = delete;
Candidate& Candidate::operator=(const Candidate&) = delete;
Candidate& Candidate::operator=(Candidate&&) = delete;
Candidate::~Candidate() = default;

Candidate::Candidate(const Sudoku &sudoku): Sudoku(sudoku),
	candidate(size * size, vector<bool>(size, true)),
	row_count(size * size, size), column_count(size * size, size),
	box_count(size * size, size), cell_count(size * size, size),
	row_index(size, vector<size_t>(size)),
	column_index(size, vector<size_t>(size)),
	box_index(size, vector<size_t>(size)), difficulty(0) {
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			row_index[i][j] = i * size + j;
			column_index[i][j] = j * size + i;
			box_index[i][j] = (i / m * m + j / n) * size + (i % m * n + j % n);
		}
	}
	for (size_t i = 0; i < size * size; ++i) {
		if (board[i] != 0) {
			Fill(i, board[i]);
		}
	}
}

void Candidate::Fill(size_t index, size_t number) {
	Fill(index / size, index % size, number);
}

void Candidate::Fill(size_t row, size_t column, size_t number) {
	size_t index = row * size + column;
	board[index] = number;
	for (size_t i = 0; i < size; ++i) {
		Remove(row, i, number);
		Remove(i, column, number);
		Remove(row, column, i + 1);
	}
	for (size_t i = 0; i < m; ++i) {
		for (size_t j = 0; j < n; ++j) {
			Remove(row / m * m + i, column / n * n + j, number);
		}
	}
}

void Candidate::Remove(size_t index, size_t number) {
	Remove(index / size, index % size, number);
}

void Candidate::Remove(size_t row, size_t column, size_t number) {
	size_t index = row * size + column;
	auto c = candidate[index][number - 1];
	if (c) {
		--row_count[row * size + number - 1];
		--column_count[column * size + number - 1];
		--box_count[(row / m * m + column / n) * size + number - 1];
		--cell_count[index];
		c = false;
	}
}

void Candidate::Remove(size_t number) {
	Remove(number / (size * size), number / size % size, number % size + 1);
}

vector<size_t>
Candidate::CommonEffectCell(size_t r1, size_t c1, size_t r2, size_t c2) {
	vector<size_t> vec;
	if (r1 == r2) {
		if (c1 == c2) {
			set<size_t> s;
			for (size_t cell: row_index[r1]) {
				s.insert(cell);
			}
			for (size_t cell: column_index[c1]) {
				s.insert(cell);
			}
			for (size_t cell: box_index[r1 / m * m + c1 / n]) {
				s.insert(cell);
			}
			s.erase(r1 * size + c1);
			for (size_t cell: s) {
				vec.push_back(cell);
			}
		} else {
			for (size_t column = 0; column < size; ++column) {
				if (column != c1 && column != c2) {
					vec.push_back(r1 * size + column);
				}
			}
		}
	} else if (c1 == c2) {
		for (size_t row = 0; row < size; ++row) {
			if (row != r1 && row != r2) {
				vec.push_back(row * size + c1);
			}
		}
	} else if (r1 / m == r2 / m) {
		if (c1 / n == c2 / n) {
			for (size_t cell: box_index[r1 / m * m + c1 / n]) {
				if ((cell / size != r1 || cell % size != c1)
					&& (cell / size != r2 || cell % size != c2)) {
					vec.push_back(cell);
				}
			}
		} else {
			for (size_t c = 0; c < n; ++c) {
				vec.push_back(r1 * size + (c2 / n * n + c));
				vec.push_back(r2 * size + (c1 / n * n + c));
			}
		}
	} else if (c1 / n == c2 / n) {
		for (size_t r = 0; r < m; ++r) {
			vec.push_back((r1 / m * m + r) * size + c2);
			vec.push_back((r2 / m * m + r) * size + c1);
		}
	} else {
		vec.push_back(r1 * size + c2);
		vec.push_back(r2 * size + c1);
	}

	sort(vec.begin(), vec.end());
	return vec;
}

vector<size_t>
Candidate::CommonEffectCell(size_t n1, size_t n2) {
	return CommonEffectCell(n1 / (size * size), n1 / size % size,
		n1 % size + 1, n2 / (size * size), n2 / size % size, n2 % size + 1);
}

vector<size_t> Candidate::CommonEffectCell(size_t r1, size_t c1, size_t n1,
	size_t r2, size_t c2, size_t n2) {
	vector<size_t> vec;
	if (n1 == n2) {
		vec = CommonEffectCell(r1, c1, r2, c2);
		for (size_t &cell: vec) {
			cell = cell * size + n1 - 1;
		}
	} else if (r1 == r2 && c1 == c2) {
		for (size_t number = 1; number <= size; ++number) {
			if (number != n1 && number != n2) {
				vec.push_back((r1 * size + c1) * size + number - 1);
			}
		}
	} else if (IsWeakChain(r1, c1, 0, r2, c2, 0)) {
		vec.push_back((r1 * size + c1) * size + n2 - 1);
		vec.push_back((r2 * size + c2) * size + n1 - 1);
		sort(vec.begin(), vec.end());
	}

	return vec;
}

bool Candidate::IsWeakChain(size_t n1, size_t n2) {
	return IsWeakChain(n1 / (size * size), n1 / size % size, n1 % size + 1,
		n2 / (size * size), n2 / size % size, n2 % size + 1);
}

bool Candidate::IsWeakChain(size_t r1, size_t c1, size_t n1, size_t r2,
	size_t c2, size_t n2) {
	if (n1 == n2) {
		if (r1 == r2) {
			return c1 != c2;
		} else if (c1 == c2) {
			return true;
		} else {
			return r1 / m == r2 / m && c1 / n == c2 / n;
		}
	} else {
		return r1 == r2 && c1 == c2;
	}
}

std::ostringstream& PrintChain(std::ostringstream &ostr,
	const BQSudoku::Candidate &candidate, std::list<size_t> number) {
	size_t size = candidate.size;
	bool is_strong_link = true;
	for (size_t num: number) {
		is_strong_link = !is_strong_link;
		if (num != number.front()) {
			ostr << (is_strong_link ? "==" : "--");
		}
		ostr << candidate.Row2Char(num / (size * size))
			<< candidate.Column2Char(num / size % size)
			<< '(' << candidate.Number2Char(num % size + 1) << ')';
	}
	return ostr;
}

string Candidate::FindNext() {
	string str = Single();
	if (!str.empty()) {
		return str;
	}
	str = HiddenSingle();
	if (!str.empty()) {
		return str;
	}
	str = NakedSingle();
	if (!str.empty()) {
		return str;
	}
	str = LockedCandidate();
	if (!str.empty()) {
		return str;
	}
	for (size_t pair_size = 2; pair_size <= size / 2; ++pair_size) {
		str = HiddenPair(pair_size);
		if (!str.empty()) {
			return str;
		}
		str = NakedPair(pair_size);
		if (!str.empty()) {
			return str;
		}
	}
	for (size_t pair_size = 2; pair_size <= size / 2; ++pair_size) {
		str = Fish(pair_size);
		if (!str.empty()) {
			return str;
		}
	}
	str = Skyscraper();
	if (!str.empty()) {
		return str;
	}
	str = _2StringKite();
	if (!str.empty()) {
		return str;
	}
	str = TurbotFish();
	if (!str.empty()) {
		return str;
	}
	str = ForcingChain();
	return str;
}

namespace {
	int Log2(int n) {
		int k = 0;
		while (n > 1) {
			n >>= 1;
			++k;
		}
		return k;
	}
}

string Candidate::Single() {
	for (size_t box = 0; box < size; ++box) {
		size_t count = 0;
		size_t row, column;
		for (size_t cell: box_index[box]) {
			if (board[cell] == 0) {
				++count;
				row = cell / size;
				column = cell % size;
			}
		}
		if (count == 1) {
			size_t number = 0;
			while (!(*this)(row, column, ++number));
			Fill(row, column, number);
			++difficulty;
			ostringstream ostr;
			ostr << "Single: " << Row2Char(row) << Column2Char(column)
				<< '=' << Number2Char(number);
			return ostr.str();
		}
	}

	for (size_t row = 0; row < size; ++row) {
		size_t count = 0;
		size_t column;
		for (size_t cell: row_index[row]) {
			if (board[cell] == 0) {
				++count;
				column = cell % size;
			}
		}
		if (count == 1) {
			size_t number = 0;
			while (!(*this)(row, column, ++number));
			Fill(row, column, number);
			++difficulty;
			ostringstream ostr;
			ostr << "Single: " << Row2Char(row) << Column2Char(column)
				<< '=' << Number2Char(number);
			return ostr.str();
		}
	}

	for (size_t column = 0; column < size; ++column) {
		size_t count = 0;
		size_t row;
		for (size_t cell: column_index[column]) {
			if (board[cell] == 0) {
				++count;
				row = cell / size;
			}
		}
		if (count == 1) {
			size_t number = 0;
			while (!(*this)(row, column, ++number));
			Fill(row, column, number);
			++difficulty;
			ostringstream ostr;
			ostr << "Single: " << Row2Char(row) << Column2Char(column)
				<< '=' << Number2Char(number);
			return ostr.str();
		}
	}
	
	return "";
}

string Candidate::NakedSingle() {
	for (size_t pos = 0; pos < size * size; ++pos) {
		if (cell_count[pos] == 1) {
			size_t number = 0;
			while (!(*this)(pos, ++number));
			Fill(pos, number);
			difficulty += 0x100;
			ostringstream ostr;
			ostr << "Naked Single: " << Row2Char(pos / size)
				<< Column2Char(pos % size) << '=' << Number2Char(number);
			return ostr.str();
		}
	}
	return "";
}

string Candidate::HiddenSingle() {
	for (size_t box = 0; box < size; ++box) {
		for (size_t number = 1; number <= size; ++number) {
			if (box_count[box * size + number - 1] == 1) {
				size_t row, column;
				for (size_t cell: box_index[box]) {
					if ((*this)(cell, number)) {
						row = cell / size;
						column = cell % size;
						break;
					}
				}
				Fill(row, column, number);
				difficulty += 0x10;
				ostringstream ostr;
				ostr << "Hidden Single (Box " << box + 1 << "): "
					<< Row2Char(row) << Column2Char(column) << '='
					<< Number2Char(number);
				return ostr.str();
			}
		}
	}

	for (size_t row = 0; row < size; ++row) {
		for (size_t number = 1; number <= size; ++number) {
			if (row_count[row * size + number - 1] == 1) {
				size_t column;
				for (size_t cell: row_index[row]) {
					if (((*this))(cell, number)) {
						column = cell % size;
						break;
					}
				}
				Fill(row, column, number);
				difficulty += 0x20;
				ostringstream ostr;
				ostr << "Hidden Single (Row " << Row2Char(row) << "): "
					<< Row2Char(row) << Column2Char(column) << '='
					<< Number2Char(number);
				return ostr.str();
			}
		}
	}

	for (size_t column = 0; column < size; ++column) {
		for (size_t number = 1; number <= size; ++number) {
			if (column_count[column * size + number - 1] == 1) {
				size_t row = 0;
				for (size_t cell: column_index[column]) {
					if ((*this)(cell, number)) {
						row = cell / size;
						break;
					}
				}
				Fill(row, column, number);
				difficulty += 0x20;
				ostringstream ostr;
				ostr << "Hidden Single (Column " << Column2Char(column) << "): "
					<< Row2Char(row) << Column2Char(column) << '='
					<< Number2Char(number);
				return ostr.str();
			}
		}
	}

	return "";
}

string Candidate::LockedCandidate() {
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
						if ((*this)(row / m * m + r, column / n * n + c,
							number)) {
							elim.push_back(make_pair(row / m * m + r,
								column / n * n + c));
						}
					}
				}
				break;
			}
			if (!elim.empty()) {
				difficulty += 0x100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number)
					<< " in Row " << Row2Char(row) << "):";
				for (auto &x: elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second)
						<< "!=" << Number2Char(number);
				}
				return ostr.str();
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
						if ((*this)(row / m * m + r, column / n * n + c,
							number)) {
							elim.push_back(make_pair(row / m * m + r,
								column / n * n + c));
						}
					}
				}
				break;
			}
			if (!elim.empty()) {
				difficulty += 0x100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number)
					<< " in Column " << Column2Char(column) << "):";
				for (auto &x: elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second)
						<< "!=" << Number2Char(number);
				}
				return ostr.str();
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
				difficulty += 0x100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number)
					<< " in Box " << box + 1 << "):";
				for (auto &x: row_elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second)
						<< "!=" << Number2Char(number);
				}
				return ostr.str();
			}
			if (!column_elim.empty()) {
				difficulty += 0x100;
				ostringstream ostr;
				ostr << "Locked Candidates (" << Number2Char(number)
					<< " in Box " << box + 1 << "):";
				for (auto &x: column_elim) {
					Remove(x.first, x.second, number);
					ostr << ' ' << Row2Char(x.first) << Column2Char(x.second)
						<< "!=" << Number2Char(number);
				}
				return ostr.str();
			}
		}
	}

	return "";
}

namespace {
	const array<string, 11> pair_name = {{"", "", "Pair", "Triple", "Quad",
		"Pent", "Hex", "Sept", "Oct", "Nov", "Dec"}};
}

string Candidate::NakedPair(size_t pair_size) {
	string name = pair_size < 11 ? pair_name[pair_size]
		: string("Pair") + Num2Str(pair_size);
	for (size_t box = 0; box < size; ++box) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (box_count[box * size + number - 1] > 0) {
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
					difficulty += 0x100 + 0x20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Naked " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Box " << box + 1 << "):";
					for (size_t cell: elim) {
						ostr << ' ' << Row2Char(cell / size)
							<< Column2Char(cell % size) << "!=";
						for (size_t number: contain) {
							if ((*this)(cell, number)) {
								Remove(cell, number);
								ostr << Number2Char(number);
							}
						}
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	for (size_t row = 0; row < size; ++row) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (row_count[row * size + number - 1] > 0) {
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
			for (size_t i = 0; i < size; ++i) {
				if (board[row * size + i] == 0) {
					for (size_t number: not_contain) {
						if ((*this)(row, i, number)) {
							--count;
							column[i] = false;
							break;
						}
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
					difficulty += 0x100 + 0x20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Naked " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Row " << Row2Char(row) << "):";
					for (size_t column: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column)
							<< "!=";
						for (size_t number: contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	for (size_t column = 0; column < size; ++column) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (column_count[column * size + number - 1] > 0) {
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
			for (size_t i = 0; i < size; ++i) {
				if (board[i * size + column] == 0) {
					for (size_t number: not_contain) {
						if ((*this)(i, column, number)) {
							--count;
							row[i] = false;
							break;
						}
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
					difficulty += 0x100 + 0x20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Naked " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Column " << Column2Char(column) << "):";
					for (size_t row: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column)
							<< "!=";
						for (size_t number: contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	return "";
}

string Candidate::HiddenPair(size_t pair_size) {
	string name = pair_size < 11 ? pair_name[pair_size]
		: string("Pair") + Num2Str(pair_size);

	for (size_t box = 0; box < size; ++box) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (box_count[box * size + number - 1] > 0) {
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
					difficulty += 0x100 + 0x20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Hidden " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Box " << box + 1 << "):";
					for (size_t k: elim) {
						size_t row = r + k / n;
						size_t column = c + k % n;
						ostr << ' ' << Row2Char(row) << Column2Char(column)
							<< "!=";
						for (size_t number: not_contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	for (size_t row = 0; row < size; ++row) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (row_count[row * size + number - 1] > 0) {
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
					difficulty += 0x100 + 0x20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Hidden " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Row " << Row2Char(row) << "):";
					for (size_t column: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column)
							<< "!=";
						for (size_t number: not_contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	for (size_t column = 0; column < size; ++column) {
		vector<size_t> num_available;
		for (size_t number = 1; number <= size; ++number) {
			if (column_count[column * size + number - 1] > 0) {
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
					difficulty += 0x100 + 0x20 * (pair_size - 2);
					ostringstream ostr;
					ostr << "Hidden " << name << " (";
					for (size_t number: contain) {
						ostr << Number2Char(number);
					}
					ostr << " in Column " << Column2Char(column) << "):";
					for (size_t row: elim) {
						ostr << ' ' << Row2Char(row) << Column2Char(column)
							<< "!=";
						for (size_t number: not_contain) {
							if ((*this)(row, column, number)) {
								Remove(row, column, number);
								ostr << Number2Char(number);
							}
						}
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	return "";
}

string Candidate::Fish(size_t fish_size) {
	static const array<string, 5> xwing_name =
		{{"", "", "X-Wing", "Swordfish", "Jellyfish"}};
	string name = fish_size < 5 ? xwing_name[fish_size]
		: Num2Str(fish_size) + string("X-Wing");
	for (size_t number = 1; number <= size; ++number) {
		vector<size_t> row_available, column_available;
		for (size_t i = 0; i < size; ++i) {
			if (row_count[i * size + number - 1] > 0) {
				row_available.push_back(i);
			}
			if (column_count[i * size + number - 1] > 0) {
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
				vector<pair<size_t, size_t>> elim;
				for (size_t column = 0; column < size; ++column) {
					if (contain[column]) {
						for (size_t i = 0; i < blank_size; ++i) if (!set[i]) {
							if ((*this)(row_available[i], column, number)) {
								elim.push_back(make_pair(row_available[i], column));
							}
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 0x1000 + 0x200 * (fish_size - 2);
					ostringstream ostr;
					ostr << name << " (" << Number2Char(number) << " in Row ";
					for (size_t row: row_contain) {
						ostr << Row2Char(row);
					}
					ostr << "):";
					for (auto x: elim) {
						Remove(x.first, x.second, number);
						ostr << ' ' << Row2Char(x.first) << Column2Char(x.second)
							<< "!=" << Number2Char(number);
					}
					return ostr.str();
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
				vector<pair<size_t, size_t>> elim;
				for (size_t row = 0; row < size; ++row) if (contain[row]) {
					for (size_t i = 0; i < blank_size; ++i) if (!set[i]) {
						if ((*this)(row, column_available[i], number)) {
							elim.push_back(make_pair(row, column_available[i]));
						}
					}
				}
				if (!elim.empty()) {
					difficulty += 0x1000 + 0x200 * (fish_size - 2);
					ostringstream ostr;
					ostr << name << " (" << Number2Char(number) << " in Column ";
					for (size_t column: column_contain) {
						ostr << Column2Char(column);
					}
					ostr << "):";
					for (auto x: elim) {
						Remove(x.first, x.second, number);
						ostr << ' ' << Row2Char(x.first) << Column2Char(x.second)
							<< "!=" << Number2Char(number);
					}
					return ostr.str();
				}
			}
		} while (NextCombination(set));
	}

	return "";
}

string Candidate::Skyscraper() {
	for (size_t number = 1; number <= size; ++number) {
		vector<pair<size_t, size_t>> rows;
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row * size + number - 1] == 2) {
				size_t c = 0;
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						c = c * size + column;
					}
				}
				rows.push_back(make_pair(row, c));
			}
		}
		size_t count = rows.size();
		if (count == 0) {
			continue;
		}
		for (size_t r1 = 0; r1 < count - 1; ++r1) {
			size_t row1 = rows[r1].first;
			size_t column11 = rows[r1].second / size;
			size_t column12 = rows[r1].second % size;
			for (size_t r2 = r1 + 1; r2 < count; ++r2) {
				size_t row2 = rows[r2].first;
				if (row1 / m != row2 / m) {
					size_t column21 = rows[r2].second / size;
					size_t column22 = rows[r2].second % size;
					vector<size_t> elim;
					if (column11 == column21) {
						for (size_t cell: CommonEffectCell(row1, column12,
							row2, column22)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					} else if (column12 == column22) {
						for (size_t cell: CommonEffectCell(row1, column11,
							row2, column21)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					}
					if (!elim.empty()) {
						difficulty += 0x2000;
						ostringstream ostr;
						ostr << "Skyscraper (" << Number2Char(number)
							<< " in Row " << Row2Char(row1) << Row2Char(row2)
							<< "):";
						for (size_t cell: elim) {
							Remove(cell, number);
							ostr << ' ' << Row2Char(cell / size)
								<< Column2Char(cell % size) << "!="
								<< Number2Char(number);
						}
						return ostr.str();
					}
				}
			}
		}

		vector<pair<size_t, size_t>> columns;
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column * size + number - 1] == 2) {
				size_t r = 0;
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						r = r * size + row;
					}
				}
				columns.push_back(make_pair(column, r));
			}
		}
		count = columns.size();
		if (count == 0) {
			continue;
		}
		for (size_t c1 = 0; c1 < count - 1; ++c1) {
			size_t column1 = columns[c1].first;
			size_t row11 = columns[c1].second / size;
			size_t row12 = columns[c1].second % size;
			for (size_t c2 = c1 + 1; c2 < count; ++c2) {
				size_t column2 = columns[c2].first;
				if (column1 / n != column2 / n) {
					size_t row21 = columns[c2].second / size;
					size_t row22 = columns[c2].second % size;
					vector<size_t> elim;
					if (row11 == row21) {
						for (size_t cell: CommonEffectCell(row12, column1,
							row22, column2)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					} else if (row12 == row22) {
						for (size_t cell: CommonEffectCell(row11, column1,
							row21, column2)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					}
					if (!elim.empty()) {
						difficulty += 0x2000;
						ostringstream ostr;
						ostr << "Skyscraper (" << Number2Char(number)
							<< " in Column " << Column2Char(column1)
							<< Column2Char(column2) << "):";
						for (size_t cell: elim) {
							Remove(cell, number);
							ostr << ' ' << Row2Char(cell / size)
								<< Column2Char(cell % size) << "!="
								<< Number2Char(number);
						}
						return ostr.str();
					}
				}
			}
		}
	}

	return "";
}

string Candidate::_2StringKite() {
	for (size_t number = 1; number <= size; ++number) {
		vector<pair<size_t, size_t>> rows, columns;
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row * size + number - 1] == 2) {
				size_t c = 0;
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						c = c * size + column;
					}
				}
				rows.push_back(make_pair(row, c));
			}
		}
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column * size + number - 1] == 2) {
				size_t r = 0;
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						r = r * size + row;
					}
				}
				columns.push_back(make_pair(column, r));
			}
		}
		size_t rcount = rows.size();
		size_t ccount = columns.size();
		for (size_t r = 0; r < rcount; ++r) {
			size_t row1 = rows[r].first;
			size_t column11 = rows[r].second / size;
			size_t column12 = rows[r].second % size;
			for (size_t c = 0; c < ccount; ++c) {
				size_t column2 = columns[c].first;
				size_t row21 = columns[c].second / size;
				size_t row22 = columns[c].second % size;
				if (row1 / m == row21 / m && row1 != row21) {
					if (column2 / n == column11 / n && column2 != column11) {
						if ((*this)(row22, column12, number)) {
							Remove(row22, column12, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2Char(number)
								<< " in Row " << Row2Char(row1)
								<< " Column " << Column2Char(column2)
								<< "): " << Row2Char(row22)
								<< Column2Char(column12) << "!="
								<< Number2Char(number);
							return ostr.str();
						}
					} else if (column2 / n == column12 / n
						&& column2 != column12) {
						if ((*this)(row22, column11, number)) {
							Remove(row22, column11, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2Char(number)
								<< " in Row " << Row2Char(row1)
								<< " Column " << Column2Char(column2)
								<< "): " << Row2Char(row22)
								<< Column2Char(column11) << "!="
								<< Number2Char(number);
							return ostr.str();
						}
					}
				} else if (row1 / m == row22 / m && row1 != row22) {
					if (column2 / n == column11 / n && column2 != column11) {
						if ((*this)(row21, column12, number)) {
							Remove(row21, column12, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2Char(number)
								<< " in Row " << Row2Char(row1)
								<< " Column " << Column2Char(column2)
								<< "): " << Row2Char(row21)
								<< Column2Char(column12) << "!="
								<< Number2Char(number);
							return ostr.str();
						}
					} else if (column2 / n == column12 / n
						&& column2 != column12) {
						if ((*this)(row21, column11, number)) {
							Remove(row21, column11, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2Char(number)
								<< " in Row " << Row2Char(row1)
								<< " Column " << Column2Char(column2)
								<< "): " << Row2Char(row21)
								<< Column2Char(column11) << "!="
								<< Number2Char(number);
							return ostr.str();
						}
					}
				}
			}
		}
	}

	return "";
}

string Candidate::TurbotFish() {
	for (size_t number = 1; number <= size; ++number) {
		vector<pair<size_t, size_t>> boxes, rows, columns;
		for (size_t box = 0; box < size; ++box) {
			if (box_count[box * size + number - 1] == 2) {
				size_t c = 0;
				for (size_t cell: box_index[box]) {
					if ((*this)(cell, number)) {
						c = c * size * size + cell;
					}
				}
				boxes.push_back(make_pair(box, c));
			}
		}
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row * size + number - 1] == 2) {
				size_t c = 0;
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						c = c * size + column;
					}
				}
				rows.push_back(make_pair(row, c));
			}
		}
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column * size + number - 1] == 2) {
				size_t r = 0;
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						r = r * size + row;
					}
				}
				columns.push_back(make_pair(column, r));
			}
		}
		size_t bcount = boxes.size();
		size_t rcount = rows.size();
		size_t ccount = columns.size();
		for (size_t b = 0; b < bcount; ++b) {
			size_t row11 = boxes[b].second / (size * size) / size;
			size_t column11 = boxes[b].second / (size * size) % size;
			size_t row12 = boxes[b].second % (size * size) / size;
			size_t column12 = boxes[b].second % (size * size) % size;
			for (size_t r = 0; r < rcount; ++r) {
				size_t row2 = rows[r].first;
				size_t column21 = rows[r].second / size;
				size_t column22 = rows[r].second % size;
				if (column11 == column21) {
					if (row11 / m != row2 / m && column12 != column22) {
						if ((*this)(row12, column22, number)) {
							Remove(row12, column22, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Row " << Row2Char(row2) << "): "
								<< Row2Char(row12) << Column2Char(column22)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				} else if (column11 == column22) {
					if (row11 / m != row2 / m && column12 != column21) {
						if ((*this)(row12, column21, number)) {
							Remove(row12, column21, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Row " << Row2Char(row2) << "): "
								<< Row2Char(row12) << Column2Char(column21)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				} else if (column12 == column21) {
					if (row12 / m != row2 / m && column11 != column22) {
						if ((*this)(row11, column22, number)) {
							Remove(row11, column22, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Row " << Row2Char(row2) << "): "
								<< Row2Char(row11) << Column2Char(column22)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				} else if (column12 == column22) {
					if (row12 / m != row2 / m && column11 != column21) {
						if ((*this)(row11, column21, number)) {
							Remove(row11, column21, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Row " << Row2Char(row2) << "): "
								<< Row2Char(row11) << Column2Char(column21)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				}
			}
			for (size_t c = 0; c < ccount; ++c) {
				size_t column2 = columns[c].first;
				size_t row21 = columns[c].second / size;
				size_t row22 = columns[c].second % size;
				if (row11 == row21) {
					if (column11 / n != column2 / n && row12 != row22) {
						if ((*this)(row22, column12, number)) {
							Remove(row22, column12, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Column " << Column2Char(column2) << "): "
								<< Row2Char(row22) << Column2Char(column12)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				} else if (row11 == row22) {
					if (column11 / n != column2 / n && row12 != row21) {
						if ((*this)(row21, column12, number)) {
							Remove(row21, column12, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Column " << Column2Char(column2) << "): "
								<< Row2Char(row21) << Column2Char(column12)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				} else if (row12 == row21) {
					if (column12 / n != column2 / n && row11 != row22) {
						if ((*this)(row22, column11, number)) {
							Remove(row22, column11, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first + 1
								<< " and Column " << Column2Char(column2) << "): "
								<< Row2Char(row22) << Column2Char(column11)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				} else if (row12 == row22) {
					if (column12 / n != column2 / n && row11 != row21) {
						if ((*this)(row21, column11, number)) {
							Remove(row21, column11, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2Char(number)
								<< " in Box " << boxes[b].first
								<< " and Column " << Column2Char(column2) << "): "
								<< Row2Char(row21) << Column2Char(column11)
								<< "!=" << Number2Char(number);
							return ostr.str();
						}
					}
				}
			}
		}
	}

	return "";
}

string Candidate::ForcingChain() {
	unordered_map<size_t, list<size_t>> original_strong_link;
	for (size_t number = 1; number <= size; ++number) {
		for (size_t box = 0; box < size; ++box) {
			if (box_count[box * size + number - 1] == 2) {
				size_t c[2], count = 0;
				for (size_t cell: box_index[box]) {
					if ((*this)(cell, number)) {
						c[count++] = cell * size + number - 1;
					}
				}
				size_t key1 = c[0] * size * size * size + c[1];
				size_t key2 = c[1] * size * size * size + c[0];
				original_strong_link[key1] = {c[0], c[1]};
				original_strong_link[key2] = {c[1], c[0]};
			}
		}
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row * size + number - 1] == 2) {
				size_t c[2], count = 0;
				for (size_t cell: row_index[row]) {
					if ((*this)(cell, number)) {
						c[count++] = cell * size + number - 1;
					}
				}
				size_t key1 = c[0] * size * size * size + c[1];
				size_t key2 = c[1] * size * size * size + c[0];
				original_strong_link[key1] = {c[0], c[1]};
				original_strong_link[key2] = {c[1], c[0]};
			}
		}
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column * size + number - 1] == 2) {
				size_t c[2], count = 0;
				for (size_t cell: column_index[column]) {
					if ((*this)(cell, number)) {
						c[count++] = cell * size + number - 1;
					}
				}
				size_t key1 = c[0] * size * size * size + c[1];
				size_t key2 = c[1] * size * size * size + c[0];
				original_strong_link[key1] = {c[0], c[1]};
				original_strong_link[key2] = {c[1], c[0]};
			}
		}
	}
	for (size_t cell = 0; cell < size * size; ++cell) {
		if (cell_count[cell] == 2) {
			size_t c[2], count = 0;
			for (size_t number = 1; number <= size; ++number) {
				if ((*this)(cell, number)) {
					c[count++] = cell * size + number - 1;
				}
			}
			size_t key1 = c[0] * size * size * size + c[1];
			size_t key2 = c[1] * size * size * size + c[0];
			original_strong_link[key1] = {c[0], c[1]};
			original_strong_link[key2] = {c[1], c[0]};
		}
	}
	deque<unordered_map<size_t, list<size_t>>> strong_link(1,
		original_strong_link);

	bool ok = true;
	while (ok) {
		ok = false;
		unordered_map<size_t, list<size_t>> link;
		size_t original_size = strong_link.size();
		for (size_t d1 = 0; d1 <= (original_size - 1) / 2; ++d1) {
			for (const auto &link1: strong_link[d1]) {
				size_t num11 = link1.first / (size * size * size);
				size_t num12 = link1.first % (size * size * size);
				const list<size_t> &l1 = link1.second;
				if (num11 > num12) {
					continue;
				}
				size_t d2 = original_size - d1 - 1;
				for (const auto &link2: strong_link[d2]) {
					size_t num21 = link2.first / (size * size * size);
					size_t num22 = link2.first % (size * size * size);
					const list<size_t> &l2 = link2.second;
					if (num21 > num22) {
						continue;
					}
					if (num11 == num21 || num11 == num22
						|| num12 == num21 || num12 == num22) {
						continue;
					}
					if (IsWeakChain(num11, num21)) {
						size_t key1 = num12 * size * size * size + num22;
						size_t key2 = num22 * size * size * size + num12;
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size();
							++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							list<size_t> new_list1(l1.crbegin(), l1.crend());
							new_list1.insert(new_list1.end(),
								l2.cbegin(), l2.cend());
							link[key1] = new_list1;
							list<size_t> new_list2(l2.crbegin(), l2.crend());
							new_list2.insert(new_list2.end(),
								l1.cbegin(), l1.cend());
							link[key2] = new_list2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_list1;
								(strong_link.back())[key2] = new_list2;
							}
							vector<size_t> common_effect_cell
								= CommonEffectCell(num12, num22);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								bool is_xchain = true;
								for (size_t x: new_list1) {
									if (x % size != new_list1.front() % size) {
										is_xchain = false;
										break;
									}
								}
								if (is_xchain) {
									difficulty += 0x2000 +
										0x800 * Log2(new_list1.size() - 2);
								} else {
									difficulty += 0x1000 +
										0x1000 * Log2(new_list1.size());
									if (new_list1.front() % size
										!= new_list1.back() % size) {
										difficulty += 0x1000;
									}
								}
								ostringstream ostr;
								ostr << (is_xchain ? "X-Chain" : "AIC") << " (";
								PrintChain(ostr, *this, new_list1);
								ostr << "):";
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' '
										<< Row2Char(cell / (size * size))
										<< Column2Char(cell / size % size)
										<< "!=" << Number2Char(cell % size + 1);
								}
								return ostr.str();
							}
						}
					} else if (IsWeakChain(num11, num22)) {
						size_t key1 = num12 * size * size * size + num21;
						size_t key2 = num21 * size * size * size + num12;
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size();
							++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							list<size_t> new_list1(l1.crbegin(), l1.crend());
							new_list1.insert(new_list1.end(),
								l2.crbegin(), l2.crend());
							link[key1] = new_list1;
							list<size_t> new_list2 = l2;
							new_list2.insert(new_list2.end(),
								l1.cbegin(), l1.cend());
							link[key2] = new_list2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_list1;
								(strong_link.back())[key2] = new_list2;
							}
							vector<size_t> common_effect_cell
								= CommonEffectCell(num12, num21);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								bool is_xchain = true;
								for (size_t x: new_list1) {
									if (x % size != new_list1.front() % size) {
										is_xchain = false;
										break;
									}
								}
								if (is_xchain) {
									difficulty += 0x2000 +
										0x800 * Log2(new_list1.size() - 2);
								} else {
									difficulty += 0x1000 +
										0x1000 * Log2(new_list1.size());
									if (new_list1.front() % size
										!= new_list1.back() % size) {
										difficulty += 0x1000;
									}
								}
								ostringstream ostr;
								ostr << (is_xchain ? "X-Chain" : "AIC") << " (";
								PrintChain(ostr, *this, new_list1);
								ostr << "):";
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' '
										<< Row2Char(cell / (size * size))
										<< Column2Char(cell / size % size)
										<< "!=" << Number2Char(cell % size + 1);
								}
								return ostr.str();
							}
						}
					} else if (IsWeakChain(num12, num21)) {
						size_t key1 = num11 * size * size * size + num22;
						size_t key2 = num22 * size * size * size + num11;
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size();
							++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							list<size_t> new_list1(l1);
							new_list1.insert(new_list1.end(),
								l2.cbegin(), l2.cend());
							link[key1] = new_list1;
							list<size_t> new_list2(l2.crbegin(), l2.crend());
							new_list2.insert(new_list2.end(),
								l1.crbegin(), l1.crend());
							link[key2] = new_list2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_list1;
								(strong_link.back())[key2] = new_list2;
							}
							vector<size_t> common_effect_cell
								= CommonEffectCell(num11, num22);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								bool is_xchain = true;
								for (size_t x: new_list1) {
									if (x % size != new_list1.front() % size) {
										is_xchain = false;
										break;
									}
								}
								if (is_xchain) {
									difficulty += 0x2000 +
										0x800 * Log2(new_list1.size() - 2);
								} else {
									difficulty += 0x1000 +
										0x1000 * Log2(new_list1.size());
									if (new_list1.front() % size
										!= new_list1.back() % size) {
										difficulty += 0x1000;
									}
								}
								ostringstream ostr;
								ostr << (is_xchain ? "X-Chain" : "AIC") << " (";
								PrintChain(ostr, *this, new_list1);
								ostr << "):";
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' '
										<< Row2Char(cell / (size * size))
										<< Column2Char(cell / size % size)
										<< "!=" << Number2Char(cell % size + 1);
								}
								return ostr.str();
							}
						}
					} else if (IsWeakChain(num12, num22)) {
						size_t key1 = num11 * size * size * size + num21;
						size_t key2 = num21 * size * size * size + num11;
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size();
							++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							list<size_t> new_list1(l1);
							new_list1.insert(new_list1.end(),
								l2.crbegin(), l2.crend());
							link[key1] = new_list1;
							list<size_t> new_list2(l2);
							new_list2.insert(new_list2.end(),
								l1.crbegin(), l1.crend());
							link[key2] = new_list2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_list1;
								(strong_link.back())[key2] = new_list2;
							}
							vector<size_t> common_effect_cell
								= CommonEffectCell(num11, num21);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								bool is_xchain = true;
								for (size_t x: new_list1) {
									if (x % size != new_list1.front() % size) {
										is_xchain = false;
										break;
									}
								}
								if (is_xchain) {
									difficulty += 0x2000 +
										0x800 * Log2(new_list1.size() - 2);
								} else {
									difficulty += 0x1000 +
										0x1000 * Log2(new_list1.size());
									if (new_list1.front() % size
										!= new_list1.back() % size) {
										difficulty += 0x1000;
									}
								}
								ostringstream ostr;
								ostr << (is_xchain ? "X-Chain" : "AIC") << " (";
								PrintChain(ostr, *this, new_list1);
								ostr << "):";
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' '
										<< Row2Char(cell / (size * size))
										<< Column2Char(cell / size % size)
										<< "!=" << Number2Char(cell % size + 1);
								}
								return ostr.str();
							}
						}
					}
				}
			}
		}
	}

	return "";
}
