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
	board(size * size) {}

Sudoku::Sudoku(size_t n, const string &str): Sudoku(n, n, str) {}

Sudoku::Sudoku(size_t m, size_t n, const string &str): Sudoku(m, n) {
	for (size_t i = 0; i < size * size; ++i) {
		if (str[i] != '.' && str[i] != '0') {
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
	size_t size = sudoku.size;
	for (size_t i = 0; i < size; ++i) {
		if (i != 0) {
			ostr << endl;
		}
		for (size_t j = 0; j < size; ++j) {
			ostr << sudoku.Number2Char(sudoku(i, j)) << ' ';
		}
	}
	return ostr;
}