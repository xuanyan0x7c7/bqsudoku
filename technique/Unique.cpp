#include <sstream>
#include "Unique.h"
using std::make_pair;
using std::ostringstream;
using std::size_t;
using std::string;
using std::vector;


Unique::Unique(const Unique&) = default;
Unique::Unique(Unique&&) = default;
Unique& Unique::operator =(const Unique&) = delete;
Unique& Unique::operator =(Unique&&) = delete;
Unique::~Unique() = default;

Unique::Unique(Candidate &sudoku): Technique(sudoku) {}

Technique::HintType Unique::GetHint() {
	Technique::HintType hint;
	hint = BivalueUniversalGrave();
	return hint;
}
/*
Technique::HintType Candidate::UniqueLoop() {
	vector<vector<size_t>> unique_loop;
	vector<bool> visited(size * size, false);
	for (size_t i = 0; i < size * size; ++i) {
		if (board[i] != 0) {
			visited[i] = true;
		}
	}
	for (size_t i = 0; i < size * size; ++i) {
		if (!visited[i]) {
			if (cell_count[i] == 2) {
				vector<vector<size_t>> loop = FindUniqueLoop(visited, i);
				if (!loop.empty()) {
					unique_loop.push_back(move(loop));
				}
			}
		}
	}

	return make_pair("", false);
}
*/
Technique::HintType Unique::BivalueUniversalGrave() {
	size_t bug1_count = 0;
	size_t row, column;
	for (size_t i = 0; i < size * size; ++i) {
		if (cell_count[i] == 3) {
			++bug1_count;
			row = i / size;
			column = i % size;
		} else if (cell_count[i] > 3) {
			return make_pair("", false);
		}
	}

	if (bug1_count == 1) {
		size_t number = 0;
		while (row_count[row][number++] != 3);
		difficulty += 250;
		Fill(row, column, number);
		ostringstream ostr;
		ostr << "Bivalue Universal Grave +1: " << Row2Char(row) << Column2Char(column) << "=" << Number2Char(number);
		return make_pair(ostr.str(), true);
	}

	return make_pair("", false);
}
