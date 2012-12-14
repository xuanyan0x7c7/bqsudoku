#include <algorithm>
#include <set>
#include <sstream>
#include "Unique.h"
using std::make_pair;
using std::move;
using std::ostringstream;
using std::set;
using std::size_t;
using std::sort;
using std::string;
using std::vector;


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

Unique::Unique(const Unique&) = default;
Unique::Unique(Unique&&) = default;
Unique& Unique::operator =(const Unique&) = delete;
Unique& Unique::operator =(Unique&&) = delete;
Unique::~Unique() = default;

Unique::Unique(Candidate &sudoku): Technique(sudoku) {}

Technique::HintType Unique::GetHint() {
	Technique::HintType hint = UniqueLoop();
	if (!hint.first.empty()) {
		return hint;
	}
	return hint = BivalueUniversalGrave();
}

const vector<size_t>& Unique::RegionIndex(size_t region) const {
	if (region < size) {
		return box_index[region];
	} else if (region < 2 * size) {
		return row_index[region - size];
	} else {
		return column_index[region - 2 * size];
	}
}

const vector<size_t>& Unique::RegionCount(size_t region) const{
	if (region < size) {
		return box_count[region];
	} else if (region < 2 * size) {
		return row_count[region - size];
	} else {
		return column_count[region - 2 * size];
	}
}

Technique::HintType Unique::UniqueLoop() {
	vector<Loop> unique_loop;
	for (size_t i = 0; i < size * size; ++i) {
		if (cell_count[i] == 2) {
			size_t n[2];
			size_t count = 0;
			for (size_t num = 1; num <= size; ++num) {
				if ((*this)(i, num)) {
					n[count++] = num;
				}
			}
			Loop loop;
			loop.cell.push_back(i);
			loop.set.insert(i);
			loop.n1 = n[0];
			loop.n2 = n[1];
			FindLoop(unique_loop, loop, 2, vector<bool>(size), -1);
		}
	}
	sort(unique_loop.begin(), unique_loop.end());
	for (const Loop &loop: unique_loop) {
		if (loop.type == 1) {
			size_t special_cell;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2) {
					special_cell = cell;
					break;
				}
			}
			ostringstream ostr;
			ostr << "Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " Type I ("
				<< Number2String(loop.n1) << Number2String(loop.n2) << " in ";
			for (size_t cell: loop.cell) {
				ostr << Cell2String(cell) << "--";
			}
			ostr << Cell2String(loop.cell.front()) << "): ";
			if (cell_count[special_cell] == 3) {
				difficulty += 200 + 50 * Log2(loop.cell.size() - 3);
				size_t number = 1;
				while (number == loop.n1 || number == loop.n2 || !(*this)(special_cell, number)) {
					++number;
				}
				Fill(special_cell, number);
				ostr << Cell2String(special_cell) << '=' << Number2String(number);
				return make_pair(ostr.str(), true);
			} else {
				difficulty += 250 + 50 * Log2(loop.cell.size() - 3);
				Remove(special_cell, loop.n1);
				Remove(special_cell, loop.n2);
				ostr << Cell2String(special_cell) << "!=" << Number2String(loop.n1) << Number2String(loop.n2);
				return make_pair(ostr.str(), false);
			}
		} else if (loop.type == 2) {
			vector<size_t> special_cell;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2) {
					special_cell.push_back(cell);
				}
			}
			size_t number = 1;
			while (number == loop.n1 || number == loop.n2 || !(*this)(special_cell.front(), number)) {
				++number;
			}
			vector<size_t> common_effect_cell = CommonEffectCell(special_cell.front() * size, special_cell.back() * size);
			vector<size_t> elim;
			for (size_t cell: common_effect_cell) {
				cell /= size;
				if ((*this)(cell, number)) {
					bool ok = true;
					for (size_t c: special_cell) {
						if (!weak_chain[c * size][cell * size]) {
							ok = false;
							break;
						}
					}
					if (ok) {
						elim.push_back(cell);
					}
				}
			}
			if (!elim.empty()) {
				difficulty += 350 + 50 * Log2(loop.cell.size() - 3);
				ostringstream ostr;
				ostr << "Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " Type II ("
					<< Number2String(loop.n1) << Number2String(loop.n2) << " in ";
				for (size_t cell: loop.cell) {
					ostr << Cell2String(cell) << "--";
				}
				ostr << Cell2String(loop.cell.front()) << "):";
				for (size_t cell: elim) {
					Remove(cell, number);
					ostr << ' ' << Cell2String(cell) << "!=" << Number2String(number);
				}
				return make_pair(ostr.str(), false);
			}
		}
	}

	return make_pair("", false);
}

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
		ostr << "Bivalue Universal Grave +1: " << Cell2String(row * size + column) << "=" << Number2String(number);
		return make_pair(ostr.str(), true);
	}

	return make_pair("", false);
}

void Unique::FindLoop(vector<Loop> &results, Loop &loop, size_t allowed, const vector<bool> &extra, int last_region_type) const {
	size_t next = loop.cell.back();
	size_t row = next / size;
	size_t column = next % size;
	size_t regions[3] = {row / m * m + column / n, size + row, 2 * size + column};
	for (int i = 0; i < 3; ++i) if (i != last_region_type) {
		size_t region = regions[i];
		for (size_t cell: RegionIndex(region)) {
			if (loop.cell.size() >= 4 && cell == loop.cell.front()) {
				bool found = false;
				for (const Loop &original_loop: results) {
					if (loop.set == original_loop.set) {
						found = true;
						break;
					}
				}
				if (!found) {
					switch (GetType(loop)) {
						case 1:
							loop.type = 1;
							results.push_back(loop);
							break;
						case 2:
							loop.type = 2;
							results.push_back(loop);
							break;
						case 3:
							loop.type = 2;
							results.push_back(loop);
							loop.type = 4;
							results.push_back(loop);
							break;
						case 4:
							loop.type = 3;
							results.push_back(loop);
							loop.type = 4;
							results.push_back(loop);
							break;
						default:
							break;
					}
				}
			} else if (loop.set.find(cell) == loop.set.end()) {
				vector<bool> new_extra(extra);
				if ((*this)(cell, loop.n1) && (*this)(cell, loop.n2)) {
					for (size_t number = 1; number <= size; ++number) {
						if (number != loop.n1 && number != loop.n2 && (*this)(cell, number)) {
							new_extra[number - 1] = true;
						}
					}
					size_t extra_size = 0;
					for (bool b: new_extra) if (b) {
						++extra_size;
					}
					if (cell_count[cell] == 2 || extra_size == 1 || allowed > 0) {
						loop.cell.push_back(cell);
						loop.set.insert(cell);
						FindLoop(results, loop, cell_count[cell] > 2 ? allowed - 1 : allowed, new_extra, i);
					}
				}
			}
		}
	}
	loop.set.erase(loop.cell.back());
	loop.cell.pop_back();
}

int Unique::GetType(const Loop &loop) const {
	set<size_t> odd, even;
	bool is_odd = false;
	for (size_t cell: loop.cell) {
		size_t row = cell / size;
		size_t column = cell % size;
		size_t regions[3] = {row / m * m + column / n, size + row, 2 * size + column};
		for (size_t region: regions) {
			set<size_t> &s = is_odd ? odd : even;
			if (s.find(region) == s.end()) {
				s.insert(region);
			} else {
				return 0;
			}
		}
		is_odd = !is_odd;
	}
	if (odd != even) {
		return 0;
	}

	vector<size_t> extra_cell;
	for (size_t cell: loop.cell) {
		if (cell_count[cell] > 2) {
			extra_cell.push_back(cell);
		}
	}
	if (extra_cell.size() == 1) {
		return 1;
	} else if (extra_cell.size() > 2) {
		return 2;
	} else {
		vector<bool> potential(size);
		for (size_t number = 1; number <= size; ++number) {
			if ((*this)(extra_cell.front(), number) || (*this)(extra_cell.back(), number)) {
				potential[number - 1] = true;
			}
		}
		potential[loop.n1 - 1] = potential[loop.n2 - 1] = false;
		size_t count = 0;
		for (bool b: potential) if (b) {
			++count;
		}
		return count == 1 ? 3 : 4;
	}
}
