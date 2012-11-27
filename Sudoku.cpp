#include <cstring>
#include <ostream>
#include "Sudoku.h"
using namespace BQSudoku;
using std::endl;
using std::ostream;
using std::size_t;
using std::string;
using std::vector;


Sudoku::Sudoku(const Sudoku&) = default;
Sudoku::Sudoku(Sudoku&&) = default;
Sudoku& Sudoku::operator=(const Sudoku&) = default;
Sudoku& Sudoku::operator=(Sudoku&&) = default;
Sudoku::~Sudoku() = default;

Sudoku::Sudoku(): Sudoku(3, 3) {}

Sudoku::Sudoku(size_t n): Sudoku(n, n) {}

Sudoku::Sudoku(size_t m, size_t n): m(m), n(n), size(m * n),
	board(size * size), given(size * size) {}

Sudoku::Sudoku(size_t n, const string &str): Sudoku(n, n, str) {}

Sudoku::Sudoku(size_t m, size_t n, const string &str): Sudoku(m, n) {
	for (size_t i = 0; i < size * size; ++i) {
		if (str[i] != '.' && str[i] != '0') {
			given[i] = true;
			if (isdigit(str[i])) {
				board[i] = str[i] - '0';
			} else {
				board[i] = toupper(str[i]) - 'A' + 1;
			}
		}
	}
}

bool Sudoku::Solved() const {
	for (size_t x: board) {
		if (x == 0) {
			return false;
		}
	}
	return true;
}

char Sudoku::Row2Char(size_t row) const {
	return static_cast<char>('A' + row);
}

char Sudoku::Column2Char(size_t column) const {
	if (size <= 9) {
		return static_cast<char>(column + '1');
	} else {
		return static_cast<char>('a' + column);
	}
}

char Sudoku::Number2Char(size_t number) const {
	if (number == 0) {
		return '.';
	} else if (size <= 9) {
		return static_cast<char>(number + '0');
	} else {
		return static_cast<char>('A' + number - 1);
	}
}

ostream& operator<<(ostream &ostr, const Sudoku &sudoku) {
	size_t m = sudoku.m;
	size_t n = sudoku.n;
	for (size_t i = 0; i < m; ++i) {
		ostr << '+';
		for (size_t j = 0; j < 2 * n + 1; ++j) {
			ostr << '-';
		}
	}
	ostr << '+' << endl;
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < m; ++j) {
			for (size_t k = 0; k < m; ++k) {
				ostr << "| ";
				for (size_t l = 0; l < n; ++l) {
					size_t row = i * m + j;
					size_t column = k * n + l;
					char number = sudoku.Number2Char(sudoku(row, column));
					if (sudoku.given[row * sudoku.size + column]) {
						ostr << "\033[1;34m" << number << "\033[0m";
					} else {
						ostr << number;
					}
					ostr << ' ';
				}
			}
			ostr << '|' << endl;
		}
		for (size_t j = 0; j < m; ++j) {
			ostr << '+';
			for (size_t k = 0; k < 2 * n + 1; ++k) {
				ostr << '-';
			}
		}
		ostr << '+' << endl;
	}
	return ostr;
}
