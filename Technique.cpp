#include "Candidate.h"
#include "Technique.h"
using std::size_t;
using std::ostringstream;
using std::vector;


Technique::Technique(const Technique&) = default;
Technique::Technique(Technique&&) = default;
Technique& Technique::operator =(const Technique&) = delete;
Technique& Technique::operator =(Technique&&) = delete;
Technique::~Technique() = default;

Technique::Technique(Candidate &sudoku): SudokuBase(sudoku.m, sudoku.n), board(sudoku.board), given(sudoku.given), candidate(sudoku.candidate),
	row_count(sudoku.row_count), column_count(sudoku.column_count), box_count(sudoku.box_count), cell_count(sudoku.cell_count),
	row_index(sudoku.row_index), column_index(sudoku.column_index), box_index(sudoku.box_index),
	row_contain(sudoku.weak_chain), column_contain(sudoku.column_contain), box_contain(sudoku.box_contain),
	weak_chain(sudoku.weak_chain), difficulty(sudoku.difficulty) {}

void Technique::Fill(size_t row, size_t column, size_t number) {
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
