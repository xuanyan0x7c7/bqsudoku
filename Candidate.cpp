#include "Candidate.h"
#include "technique/TechniqueList.h"
using std::make_pair;
using std::size_t;
using std::vector;


Candidate::Candidate(const Candidate&) = default;
Candidate::Candidate(Candidate&&) = default;
Candidate& Candidate::operator =(const Candidate&) = delete;
Candidate& Candidate::operator =(Candidate&&) = delete;

Candidate::~Candidate() {
	for (Technique *t: technique) {
		delete t;
	}
}

Candidate::Candidate(const Sudoku &sudoku): Sudoku(sudoku),
	candidate(size * size, vector<bool>(size, true)),
	row_count(size, vector<size_t>(size, size)), column_count(size, vector<size_t>(size, size)),
	box_count(size, vector<size_t>(size, size)), cell_count(size * size, size),
	row_index(size, vector<size_t>(size)), column_index(size, vector<size_t>(size)), box_index(size, vector<size_t>(size)),
	row_contain(size, vector<bool>(size * size)), column_contain(size, vector<bool>(size * size)),
	box_contain(size, vector<bool>(size * size)), row_blank(size, size), column_blank(size, size), box_blank(size, size),
	weak_chain(size * size * size, vector<bool>(size * size * size)), difficulty(0) {
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			row_index[i][j] = i * size + j;
			column_index[i][j] = j * size + i;
			box_index[i][j] = (i / m * m + j / n) * size + (i % m * n + j % n);
		}
	}

	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			row_contain[i][i * size + j] = true;
			column_contain[i][j * size + i] = true;
		}
		for (size_t r = 0; r < m; ++r) for (size_t c = 0; c < n; ++c) {
			box_contain[i][(i / m * m + r) * size + (i % m * n + c)] = true;
		}
	}

	for (size_t number = 1; number <= size; ++number) {
		for (size_t i = 0; i < size; ++i) {
			for (size_t c1: row_index[i]) for (size_t c2: row_index[i]) {
				if (c1 != c2) {
					weak_chain[c1 * size + number - 1][c2 * size + number - 1] = true;
				}
			}
			for (size_t c1: column_index[i]) for (size_t c2: column_index[i]) {
				if (c1 != c2) {
					weak_chain[c1 * size + number - 1][c2 * size + number - 1] = true;
				}
			}
			for (size_t c1: box_index[i]) for (size_t c2: box_index[i]) {
				if (c1 != c2) {
					weak_chain[c1 * size + number - 1][c2 * size + number - 1] = true;
				}
			}
		}
	}
	for (size_t i = 0; i < size * size; ++i) {
		for (size_t n1 = 1; n1 < size; ++n1) for (size_t n2 = n1 + 1; n2 <= size; ++n2) {
			weak_chain[i * size + n1 - 1][i * size + n2 - 1] = weak_chain[i * size + n2 - 1][i * size + n1 - 1] = true;
		}
	}

	for (size_t i = 0; i < size * size; ++i) {
		if (board[i] != 0) {
			Fill(i, board[i]);
		}
	}

	technique.push_back(new Single(*this));
	technique.push_back(new Lock(*this));
	technique.push_back(new Fish(*this));
	technique.push_back(new Unique(*this));
	technique.push_back(new Chain(*this));
}

void Candidate::Fill(size_t row, size_t column, size_t number) {
	--row_blank[row];
	--column_blank[column];
	--box_blank[row / m * m + column / n];
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

void Candidate::Remove(size_t row, size_t column, size_t number) {
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

Technique::HintType Candidate::GetHint() {
	Technique::HintType hint;
	for (Technique *t: technique) {
		hint = t->GetHint();
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
}
