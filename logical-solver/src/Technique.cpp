#include <algorithm>
#include <set>
#include "Candidate.h"
#include "Technique.h"
using std::size_t;
using std::ostringstream;
using std::set;
using std::sort;
using std::vector;


Technique::Technique(const Technique&) = default;
Technique::Technique(Technique&&) = default;
Technique& Technique::operator =(const Technique&) = delete;
Technique& Technique::operator =(Technique&&) = delete;
Technique::~Technique() = default;

Technique::Technique(Candidate &sudoku): SudokuBase(sudoku.m, sudoku.n), grid(sudoku.grid), given(sudoku.given), candidate(sudoku.candidate),
	row_count(sudoku.row_count), column_count(sudoku.column_count), box_count(sudoku.box_count), cell_count(sudoku.cell_count),
	row_index(sudoku.row_index), column_index(sudoku.column_index), box_index(sudoku.box_index),
	row_contain(sudoku.weak_link), column_contain(sudoku.column_contain), box_contain(sudoku.box_contain),
	row_blank(sudoku.row_blank), column_blank(sudoku.column_blank), box_blank(sudoku.box_blank),
	weak_link(sudoku.weak_link), difficulty(sudoku.difficulty) {}

void Technique::Fill(size_t row, size_t column, size_t number) {
	--row_blank[row];
	--column_blank[column];
	--box_blank[row / m * m + column / n];
	size_t index = row * size + column;
	grid[index] = number;
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

void Technique::Remove(size_t row, size_t column, size_t number) {
	size_t index = row * size + column;
	auto c = candidate[index][number - 1];
	if (c) {
		--row_count[row][number - 1];
		--column_count[column][number - 1];
		--box_count[row / m * m + column / n][number - 1];
		--cell_count[index];
		c = false;
	}
}

vector<size_t> Technique::CommonEffectCell(size_t r1, size_t c1, size_t r2, size_t c2) const {
	vector<size_t> vec;
	if (r1 == r2 || c1 == c2) {
		set<size_t> c;
		for (size_t cell: row_index[r1]) {
			c.insert(cell);
		}
		for (size_t cell: column_index[c1]) {
			c.insert(cell);
		}
		for (size_t cell: box_index[r1 / m * m + c1 / n]) {
			c.insert(cell);
		}
		c.erase(r1 * size + c1);
		for (size_t cell: c) {
			if (weak_link[cell * size][r2 * size * size + c2 * size]) {
				vec.push_back(cell);
			}
		}
	/*if (r1 == r2)
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
		}*/
	} else if (r1 / m == r2 / m) {
		if (c1 / n == c2 / n) {
			for (size_t cell: box_index[r1 / m * m + c1 / n]) {
				if ((!row_contain[r1][cell] || !column_contain[c1][cell]) && (!row_contain[r2][cell] || !column_contain[c2][cell])) {
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

vector<size_t> Technique::CommonEffectCell(size_t r1, size_t c1, size_t n1, size_t r2, size_t c2, size_t n2) const {
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
	} else if (weak_link[r1 * size * size + c1 * size][r2 * size * size + c2 * size]) {
		vec.push_back((r1 * size + c1) * size + n2 - 1);
		vec.push_back((r2 * size + c2) * size + n1 - 1);
		sort(vec.begin(), vec.end());
	}

	return vec;
}
