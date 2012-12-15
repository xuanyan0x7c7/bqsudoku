#include <cstring>
#include "Sudoku.h"
using std::ostream;
using std::size_t;
using std::string;
using std::vector;


Sudoku::Sudoku(const Sudoku&) = default;
Sudoku::Sudoku(Sudoku&&) = default;
Sudoku& Sudoku::operator =(const Sudoku&) = delete;
Sudoku& Sudoku::operator =(Sudoku&&) = delete;
Sudoku::~Sudoku() = default;

Sudoku::Sudoku(): Sudoku(3, 3) {}
Sudoku::Sudoku(size_t n): Sudoku(n, n) {}
Sudoku::Sudoku(size_t m, size_t n): SudokuBase(m, n), grid(size * size), given(size * size) {}
Sudoku::Sudoku(size_t n, const string &str): Sudoku(n, n, str) {}

Sudoku::Sudoku(size_t m, size_t n, const string &str): Sudoku(m, n) {
	for (size_t i = 0; i < size * size; ++i) {
		if (str[i] != '.' && str[i] != '0') {
			given[i] = true;
			if (isdigit(str[i])) {
				grid[i] = str[i] - '0';
			} else {
				grid[i] = toupper(str[i]) - 'A' + 1;
			}
		}
	}
}

bool Sudoku::Solved() const {
	for (size_t x: grid) {
		if (x == 0) {
			return false;
		}
	}
	return true;
}

ostream& operator <<(ostream &ostr, const Sudoku &sudoku) {
	size_t m = sudoku.m;
	size_t n = sudoku.n;
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < m; ++j) {
			ostr << '+';
			ostr << string(2 * n + 1, '-');
		}
		ostr << "+\n";
		for (size_t j = 0; j < m; ++j) {
			for (size_t k = 0; k < m; ++k) {
				ostr << "| ";
				for (size_t l = 0; l < n; ++l) {
					size_t row = i * m + j;
					size_t column = k * n + l;
					string str = sudoku.Number2String(sudoku(row, column));
#ifdef SHOW_COLOR
					if (sudoku.given[row * sudoku.size + column]) {
						ostr << "\033[31m" << str << "\033[0m";
					} else {
						ostr << str;
					}
#else
					ostr << str;
#endif
					ostr << ' ';
				}
			}
			ostr << "|\n";
		}
	}
	for (size_t i = 0; i < m; ++i) {
		ostr << '+';
		ostr << string(2 * n + 1, '-');
	}
	return ostr << '+';
}
