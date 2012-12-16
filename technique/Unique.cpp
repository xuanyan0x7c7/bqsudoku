#include <algorithm>
#include <functional>
#include <sstream>
#include "Unique.h"
using std::function;
using std::make_pair;
using std::mem_fn;
using std::move;
using std::ostringstream;
using std::size_t;
using std::sort;
using std::swap;
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
	static const function<HintType(Unique&)> algs[3]
		= {mem_fn(&Unique::UniqueLoop), mem_fn(&Unique::AvoidableRectangle), mem_fn(&Unique::BivalueUniversalGrave)};
	for (const auto &alg: algs) {
		HintType hint = alg(*this);
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
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
			loop.set.resize(size * size);
			loop.cell.push_back(i);
			loop.set[i] = true;
			loop.var[0] = n[0];
			loop.var[1] = n[1];
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
				<< Number2String(loop.var[0]) << Number2String(loop.var[1]) << " in ";
			for (size_t cell: loop.cell) {
				ostr << Cell2String(cell) << "--";
			}
			ostr << Cell2String(loop.cell.front()) << "): ";
			difficulty += 250 + 50 * Log2(loop.cell.size() - 3);
			Remove(special_cell, loop.var[0]);
			Remove(special_cell, loop.var[1]);
			ostr << Cell2String(special_cell) << "!=" << Number2String(loop.var[0]) << Number2String(loop.var[1]);
			return make_pair(ostr.str(), false);
		} else if (loop.type == 2) {
			vector<size_t> special_cell;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2) {
					special_cell.push_back(cell);
				}
			}
			size_t number = 1;
			while (number == loop.var[0] || number == loop.var[1] || !(*this)(special_cell.front(), number)) {
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
					<< Number2String(loop.var[0]) << Number2String(loop.var[1]) << " in ";
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
		} else if (loop.type == 4) {
			size_t special_cell[2];
			bool place[2];
			size_t count = 0;
			bool p = false;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2) {
					special_cell[count] = cell;
					place[count++] = p;
				}
				p = !p;
			}
			if (special_cell[0] > special_cell[1]) {
				swap(special_cell[0], special_cell[1]);
			}
			int elim = 0;
			for (int i: {0, 1}) {
				size_t row = special_cell[i] / size;
				size_t column = special_cell[i] % size;
				size_t regions[3] = {row / m * m + column / n, size + row, 2 * size + column};
				bool ok = true;
				for (size_t region: regions) {
					if (RegionCount(region)[loop.var[i] - 1] != 2) {
						ok = false;
						break;
					}
				}
				if (ok) {
					elim = i + 1;
					break;
				}
			}
			if (elim > 0) {
				difficulty += 400 + 50 * Log2(loop.cell.size() - 3);
				size_t number = (elim == 1) ^ place[0] ^ place[1] ? loop.var[0] : loop.var[1];
				Remove(special_cell[0], number);
				Remove(special_cell[1], number);
				ostringstream ostr;
				ostr << "Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " Type IV ("
					<< Number2String(loop.var[0]) << Number2String(loop.var[1]) << " in ";
				for (size_t cell: loop.cell) {
					ostr << Cell2String(cell) << "--";
				}
				ostr << Cell2String(loop.cell.front()) << "):";
				for (size_t cell: special_cell) {
					ostr << ' ' << Cell2String(cell) << "!=" << Number2String(number);
				}
				return make_pair(ostr.str(), false);
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Unique::AvoidableRectangle() {
	for (size_t r1 = 0; r1 < size - 1; ++r1) for (size_t r2 = 1; r2 < size; ++r2) {
		for (size_t c1 = 0; c1 < size - 1; ++c1) for (size_t c2 = 1; c2 < size; ++c2) {
			if ((r1 / m == r2 / m) ^ (c1 / n == c2 / n)) {
				size_t cell[4] = {r1 * size + c1, r1 * size + c2, r2 * size + c1, r2 * size + c2};
				bool ok = true;
				for (size_t c: cell) {
					if (given[c]) {
						ok = false;
						break;
					}
				}
				if (!ok) {
					continue;
				}
				if (grid[cell[0]] > 0 && grid[cell[0]] == grid[cell[3]]) {
					if (grid[cell[1]] != 0 && (*this)(cell[2], grid[cell[1]])) {
						difficulty += 100;
						Remove(cell[2], grid[cell[1]]);
						ostringstream ostr;
						ostr << "Avoidable Rectangle (Row" << Row2String(r1) << Row2String(r2) << ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[2]) << "!=" << Number2String(grid[cell[1]]);
						return make_pair(ostr.str(), false);
					} else if (grid[cell[2]] != 0 && (*this)(cell[1], grid[cell[2]])) {
						difficulty += 100;
						Remove(cell[1], grid[cell[2]]);
						ostringstream ostr;
						ostr << "Avoidable Rectangle (Row" << Row2String(r1) << Row2String(r2) << ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[1]) << "!=" << Number2String(grid[cell[2]]);
						return make_pair(ostr.str(), false);
					}
				} else if (grid[cell[1]] > 0 && grid[cell[1]] == grid[cell[2]]) {
					if (grid[cell[0]] != 0 && (*this)(cell[3], grid[cell[0]])) {
						difficulty += 100;
						Remove(cell[3], grid[cell[0]]);
						ostringstream ostr;
						ostr << "Avoidable Rectangle (Row" << Row2String(r1) << Row2String(r2) << ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[3]) << "!=" << Number2String(grid[cell[0]]);
						return make_pair(ostr.str(), false);
					} else if (grid[cell[3]] != 0 && (*this)(cell[0], grid[cell[3]])) {
						difficulty += 100;
						Remove(cell[0], grid[cell[3]]);
						ostringstream ostr;
						ostr << "Avoidable Rectangle (Row" << Row2String(r1) << Row2String(r2) << ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[0]) << "!=" << Number2String(grid[cell[3]]);
						return make_pair(ostr.str(), false);
					}
				}
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Unique::BivalueUniversalGrave() {
	size_t bug1_count = 0;
	size_t cell;
	for (size_t i = 0; i < size * size; ++i) {
		if (cell_count[i] == 3) {
			++bug1_count;
			cell = i;
		} else if (cell_count[i] > 3) {
			return make_pair("", false);
		}
	}

	if (bug1_count == 1) {
		size_t number = 0;
		while (row_count[cell / size][number++] != 3);
		if ((*this)(cell, number)) {
			difficulty += 250;
			Fill(cell, number);
			ostringstream ostr;
			ostr << "Bivalue Universal Grave +1: " << Cell2String(cell) << '=' << Number2String(number);
			return make_pair(ostr.str(), true);
		}
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
			} else if (!loop.set[cell]) {
				vector<bool> new_extra(extra);
				if ((*this)(cell, loop.var[0]) && (*this)(cell, loop.var[1])) {
					for (size_t number = 1; number <= size; ++number) {
						if (number != loop.var[0] && number != loop.var[1] && (*this)(cell, number)) {
							new_extra[number - 1] = true;
						}
					}
					size_t extra_size = 0;
					for (bool b: new_extra) if (b) {
						++extra_size;
					}
					if (cell_count[cell] == 2 || extra_size == 1 || allowed > 0) {
						loop.cell.push_back(cell);
						loop.set[cell] = true;
						FindLoop(results, loop, cell_count[cell] > 2 ? allowed - 1 : allowed, new_extra, i);
					}
				}
			}
		}
	}
	loop.set[loop.cell.back()] = false;
	loop.cell.pop_back();
}

int Unique::GetType(const Loop &loop) const {
	vector<bool> odd(3 * size), even(3 * size);
	bool is_odd = false;
	for (size_t cell: loop.cell) {
		size_t row = cell / size;
		size_t column = cell % size;
		size_t regions[3] = {row / m * m + column / n, size + row, 2 * size + column};
		for (size_t region: regions) {
			vector<bool> &s = is_odd ? odd : even;
			if (!s[region]) {
				s[region] = true;
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
		size_t count = 0;
		for (size_t number = 1; number <= size; ++number) {
			if ((*this)(extra_cell.front(), number) || (*this)(extra_cell.back(), number)) {
				++count;
			}
		}
		return count == 3 ? 3 : 4;
	}
}
