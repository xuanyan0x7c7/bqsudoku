#include <functional>
#include <sstream>
#include "Single.h"
using std::function;
using std::mem_fn;
using std::ostringstream;
using std::size_t;
using std::make_pair;


Single::Single(const Single&) = default;
Single::Single(Single&&) = default;
Single& Single::operator =(const Single&) = delete;
Single& Single::operator =(Single&&) = delete;
Single::~Single() = default;

Single::Single(Candidate &sudoku): Technique(sudoku) {}

Technique::HintType Single::GetHint() {
	static const function<HintType(Single&)> algs[3]
		= {mem_fn(&Single::_Single), mem_fn(&Single::HiddenSingle), mem_fn(&Single::NakedSingle)};
	for (const auto &alg: algs) {
		HintType hint = alg(*this);
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
}

Technique::HintType Single::_Single() {
	for (size_t box = 0; box < size; ++box) {
		size_t count = 0;
		size_t row, column;
		for (size_t cell: box_index[box]) {
			if (grid[cell] == 0) {
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
			ostr << "Single: " << Cell2String(row * size + column) << '=' << Number2String(number);
			return make_pair(ostr.str(), true);
		}
	}

	for (size_t row = 0; row < size; ++row) {
		size_t count = 0;
		size_t column;
		for (size_t cell: row_index[row]) {
			if (grid[cell] == 0) {
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
			ostr << "Single: " << Cell2String(row * size + column) << '=' << Number2String(number);
			return make_pair(ostr.str(), true);
		}
	}

	for (size_t column = 0; column < size; ++column) {
		size_t count = 0;
		size_t row;
		for (size_t cell: column_index[column]) {
			if (grid[cell] == 0) {
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
			ostr << "Single: " << Cell2String(row * size + column) << '=' << Number2String(number);
			return make_pair(ostr.str(), true);
		}
	}
	
	return make_pair("", false);
}

Technique::HintType Single::HiddenSingle() {
	for (size_t box = 0; box < size; ++box) {
		for (size_t number = 1; number <= size; ++number) {
			if (box_count[box][number - 1] == 1) {
				size_t row, column;
				for (size_t cell: box_index[box]) {
					if ((*this)(cell, number)) {
						row = cell / size;
						column = cell % size;
						break;
					}
				}
				Fill(row, column, number);
				difficulty += 10;
				ostringstream ostr;
				ostr << "Hidden Single (Box " << box + 1 << "): " << Cell2String(row * size + column) << '=' << Number2String(number);
				return make_pair(ostr.str(), true);
			}
		}
	}

	for (size_t row = 0; row < size; ++row) {
		for (size_t number = 1; number <= size; ++number) {
			if (row_count[row][number - 1] == 1) {
				size_t column;
				for (size_t cell: row_index[row]) {
					if (((*this))(cell, number)) {
						column = cell % size;
						break;
					}
				}
				Fill(row, column, number);
				difficulty += 20;
				ostringstream ostr;
				ostr << "Hidden Single (Row " << Row2String(row) << "): " << Cell2String(row * size + column) << '=' << Number2String(number);
				return make_pair(ostr.str(), true);
			}
		}
	}

	for (size_t column = 0; column < size; ++column) {
		for (size_t number = 1; number <= size; ++number) {
			if (column_count[column][number - 1] == 1) {
				size_t row = 0;
				for (size_t cell: column_index[column]) {
					if ((*this)(cell, number)) {
						row = cell / size;
						break;
					}
				}
				Fill(row, column, number);
				difficulty += 20;
				ostringstream ostr;
				ostr << "Hidden Single (Column " << Column2String(column) << "): " << Cell2String(row * size + column) << '=' << Number2String(number);
				return make_pair(ostr.str(), true);
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Single::NakedSingle() {
	for (size_t cell = 0; cell < size * size; ++cell) {
		if (cell_count[cell] == 1) {
			size_t number = 0;
			while (!(*this)(cell, ++number));
			Fill(cell, number);
			difficulty += 100;
			ostringstream ostr;
			ostr << "Naked Single: " << Cell2String(cell) << '=' << Number2String(number);
			return make_pair(ostr.str(), true);
		}
	}

	return make_pair("", false);
}
