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
	static const function<HintType(Unique&)> algs[4]
		= {mem_fn(&Unique::UniqueLoop), mem_fn(&Unique::HiddenLoop), mem_fn(&Unique::AvoidableRectangle), mem_fn(&Unique::BivalueUniversalGrave)};
	for (const auto &alg: algs) {
		HintType hint = alg(*this);
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
}

Technique::HintType Unique::UniqueLoop() {
	vector<Loop> unique_loop;
	for (size_t i = 0; i < size * size; ++i) {
		if (cell_count[i] == 2) {
			Loop loop;
			loop.set.resize(size * size);
			loop.cell.push_back(i);
			loop.set[i] = true;
			loop.region_use.resize(3 * size);
			for (size_t region: GetRegion(i)) {
				++loop.region_use[region];
			}
			size_t count = 0;
			for (size_t num = 1; num <= size; ++num) {
				if ((*this)(i, num)) {
					loop.var[count++] = num;
				}
			}
			FindUniqueLoop(unique_loop, loop, 2, vector<bool>(size), -1);
		}
	}
	sort(unique_loop.begin(), unique_loop.end());

	for (const Loop &loop: unique_loop) {
		if (loop.type == 1) {
			size_t special_cell;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2 || !(*this)(cell, loop.var[0]) || !(*this)(cell, loop.var[1])) {
					special_cell = cell;
					break;
				}
			}
			difficulty += 250 + 50 * Log2(loop.cell.size() - 3);
			ostringstream ostr;
			ostr << "Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " Type I ("
				<< Number2String(loop.var[0]) << Number2String(loop.var[1]) << " in ";
			for (size_t cell: loop.cell) {
				ostr << Cell2String(cell) << "--";
			}
			ostr << Cell2String(loop.cell.front()) << "): " << Cell2String(special_cell) << "!=";
			for (size_t i = 0; i < 2; ++i) {
				if ((*this)(special_cell, loop.var[i])) {
					Remove(special_cell, loop.var[i]);
					ostr << Number2String(loop.var[i]);
				}
			}
			return make_pair(ostr.str(), false);
		} else if (loop.type == 2) {
			vector<size_t> special_cell;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2 || !(*this)(cell, loop.var[0]) || !(*this)(cell, loop.var[1])) {
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
						if (!weak_link[c * size][cell * size]) {
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
		} else if (loop.type == 3) {
			continue; // TODO
			size_t special_cell[2];
			size_t count = 0;
			for (size_t cell: loop.cell) {
				if (cell_count[cell] > 2 || !(*this)(cell, loop.var[0]) || !(*this)(cell, loop.var[1])) {
					special_cell[count++] = cell;
				}
			}
			if ((!(*this)(special_cell[0], loop.var[0]) && !(*this)(special_cell[1], loop.var[0]))
					|| (!(*this)(special_cell[0], loop.var[1]) && !(*this)(special_cell[1], loop.var[1]))) {
				continue;
			}
			vector<size_t> regions;
			for (size_t region: GetRegion(special_cell[0])) {
				if (RegionContain(region)[special_cell[1]]) {
					regions.push_back(region);
				}
			}
			if (regions.empty()) {
				continue;
			}
			if (special_cell[0] > special_cell[1]) {
				swap(special_cell[0], special_cell[1]);
			}
			vector<size_t> extra_value;
			for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
				if ((*this)(special_cell[0], number) || (*this)(special_cell[1], number)) {
					extra_value.push_back(number);
				}
			}
			const size_t extra_count = extra_value.size();
			for (size_t pair_size = extra_count; pair_size < size; ++pair_size) {
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Unique::HiddenLoop() {
	vector<Loop> hidden_loop;
	for (size_t i = 0; i < size * size; ++i) {
		if (cell_count[i] == 2) {
			Loop loop;
			loop.cell.push_back(i);
			loop.set.resize(size * size);
			loop.set[i] = true;
			size_t count = 0;
			for (size_t num = 1; num <= size; ++num) {
				if ((*this)(i, num)) {
					loop.var[count++] = num;
				}
			}
			bool ok1 = true, ok2 = true;
			for (size_t region: GetRegion(i)) {
				if (RegionCount(region)[loop.var[0] - 1] != 2) {
					ok1 = false;
				}
				if (RegionCount(region)[loop.var[1] - 1] != 2) {
					ok2 = false;
				}
			}
			if (ok1) {
				FindHiddenLoop(hidden_loop, loop, 0, 0, -1);
			} else if (ok2) {
				swap(loop.var[0], loop.var[1]);
				FindHiddenLoop(hidden_loop, loop, 0, 0, -1);
			}
		}
	}
	sort(hidden_loop.begin(), hidden_loop.end());

	for (Loop &loop: hidden_loop) {
		if (loop.type == 1) {
			difficulty += 300 + 50 * Log2(loop.cell.size() - 3);
			ostringstream ostr;
			ostr << "Hidden Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " (";
			for (size_t cell: loop.cell) {
				ostr << Cell2String(cell) << "--";
			}
			ostr << Cell2String(loop.cell.front()) << "):";
			vector<size_t> elim;
			for (auto iter = loop.cell.cbegin(); iter != loop.cell.cend(); iter += 2) {
				elim.push_back(*(iter + 1));
			}
			sort(elim.begin(), elim.end());
			for (size_t cell: elim) {
				Remove(cell, loop.var[0]);
				ostr << ' ' << Cell2String(cell) << "!=" << Number2String(loop.var[0]);
			}
			return make_pair(ostr.str(), false);
		} else if (loop.type == 2) {
			difficulty += 300 + 50 * Log2(loop.cell.size() - 3);
			ostringstream ostr;
			ostr << "Hidden Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " (";
			for (size_t cell: loop.cell) {
				ostr << Cell2String(cell) << "--";
			}
			ostr << Cell2String(loop.cell.front()) << "):";
			vector<size_t> elim;
			for (auto iter = loop.cell.cbegin(); iter != loop.cell.cend(); iter += 2) {
				elim.push_back(*iter);
			}
			sort(elim.begin(), elim.end());
			for (size_t cell: elim) {
				Remove(cell, loop.var[0]);
				ostr << ' ' << Cell2String(cell) << "!=" << Number2String(loop.var[0]);
			}
			return make_pair(ostr.str(), false);
		} else if (loop.type == 3) {
			size_t special_cell[2];
			size_t count = 0;
			for (auto iter = loop.cell.cbegin(); iter != loop.cell.cend(); ++iter) {
				for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
					if ((*this)(*iter, number)) {
						special_cell[count++] = *iter;
						break;
					}
				}
			}
			if (special_cell[0] > special_cell[1]) {
				swap(special_cell[0], special_cell[1]);
			}
			bool elim[2];
			for (size_t i = 0; i < 2; ++i) {
				elim[i] = ((*this)(special_cell[i], loop.var[1]));
			}
			if (elim[0] || elim[1]) {
				difficulty += 300 + 50 * Log2(loop.cell.size() - 3);
				ostringstream ostr;
				ostr << "Hidden Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " (" << Number2String(loop.var[0]) << " in ";
				for (size_t cell: loop.cell) {
					ostr << Cell2String(cell) << "--";
				}
				ostr << Cell2String(loop.cell.front()) << "):";
				for (size_t i = 0; i < 2; ++i) if (elim[i]) {
					Remove(special_cell[i], loop.var[1]);
					ostr << ' ' << Cell2String(special_cell[i]) << "!=" << Number2String(loop.var[1]);
				}
				return make_pair(ostr.str(), false);
			}
		} else if (loop.type == 4) {
			size_t special_cell;
			for (auto iter = loop.cell.cbegin(); iter != loop.cell.cend(); iter += 2) {
				for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
					if ((*this)(*iter, number)) {
						special_cell = *iter;
						break;
					}
				}
			}
			if ((*this)(special_cell, loop.var[1])) {
				difficulty += 400 + 50 * Log2(loop.cell.size() - 3);
				Remove(special_cell, loop.var[1]);
				ostringstream ostr;
				ostr << "Hidden Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " (" << Number2String(loop.var[0]) << " in ";
				for (size_t cell: loop.cell) {
					ostr << Cell2String(cell) << "--";
				}
				ostr << Cell2String(loop.cell.front()) << "): " << Cell2String(special_cell) << "!=" << Number2String(loop.var[1]);
				return make_pair(ostr.str(), false);
			}
		} else if (loop.type == 5) {
			size_t special_cell;
			for (auto iter = loop.cell.cbegin(); iter != loop.cell.cend(); iter += 2) {
				for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
					if ((*this)(*(iter + 1), number)) {
						special_cell = *(iter + 1);
						break;
					}
				}
			}
			if ((*this)(special_cell, loop.var[1])) {
				difficulty += 400 + 50 * Log2(loop.cell.size() - 3);
				Remove(special_cell, loop.var[1]);
				ostringstream ostr;
				ostr << "Hidden Unique " << (loop.cell.size() == 4 ? "Rectangle" : "Loop") << " (" << Number2String(loop.var[0]) << " in ";
				for (size_t cell: loop.cell) {
					ostr << Cell2String(cell) << "--";
				}
				ostr << Cell2String(loop.cell.front()) << "): " << Cell2String(special_cell) << "!=" << Number2String(loop.var[1]);
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
						ostr << "Avoidable Unique Rectangle (Row" << Row2String(r1) << Row2String(r2)
							<< ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[2]) << "!=" << Number2String(grid[cell[1]]);
						return make_pair(ostr.str(), false);
					} else if (grid[cell[2]] != 0 && (*this)(cell[1], grid[cell[2]])) {
						difficulty += 100;
						Remove(cell[1], grid[cell[2]]);
						ostringstream ostr;
						ostr << "Avoidable Unique Rectangle (Row" << Row2String(r1) << Row2String(r2)
							<< ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[1]) << "!=" << Number2String(grid[cell[2]]);
						return make_pair(ostr.str(), false);
					}
				} else if (grid[cell[1]] > 0 && grid[cell[1]] == grid[cell[2]]) {
					if (grid[cell[0]] != 0 && (*this)(cell[3], grid[cell[0]])) {
						difficulty += 100;
						Remove(cell[3], grid[cell[0]]);
						ostringstream ostr;
						ostr << "Avoidable Unique Rectangle (Row" << Row2String(r1) << Row2String(r2)
							<< ", Column" << Column2String(c1) << Column2String(c2)
							<< "): " << Cell2String(cell[3]) << "!=" << Number2String(grid[cell[0]]);
						return make_pair(ostr.str(), false);
					} else if (grid[cell[3]] != 0 && (*this)(cell[0], grid[cell[3]])) {
						difficulty += 100;
						Remove(cell[0], grid[cell[3]]);
						ostringstream ostr;
						ostr << "Avoidable Unique Rectangle (Row" << Row2String(r1) << Row2String(r2)
							<< ", Column" << Column2String(c1) << Column2String(c2)
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

void Unique::FindUniqueLoop(vector<Loop> &results, Loop &loop, size_t allowed, const vector<bool> &extra, int last_region_type) const {
	size_t next = loop.cell.back();
	for (int i = 0; i < 3; ++i) if (i != last_region_type) {
		size_t region = GetRegion(next)[i];
		for (size_t cell: RegionIndex(region)) if (cell != next) {
			if (loop.cell.size() >= 4 && cell == loop.cell.front()) {
				bool found = false;
				for (const Loop &original_loop: results) {
					if (loop.set == original_loop.set) {
						found = true;
						break;
					}
				}
				if (!found) {
					loop.type = GetUniqueType(loop);
					if (loop.type != 0) {
						results.push_back(loop);
					}
				}
			} else if (!loop.set[cell] && grid[cell] == 0) {
				if ((*this)(cell, loop.var[0]) || (*this)(cell, loop.var[1])) {
					bool ok = true;
					for (size_t r: GetRegion(cell)) {
						if (loop.region_use[r] == 2) {
							ok = false;
							break;
						}
						const vector<size_t> &region_count = RegionCount(r);
						if (region_count[loop.var[0] - 1] == 0 || region_count[loop.var[1] - 1] == 0) {
							ok = false;
							break;
						}
					}
					if (!ok) {
						continue;
					}
					vector<bool> new_extra(extra);
					for (size_t number = 1; number <= size; ++number) {
						if (number != loop.var[0] && number != loop.var[1] && (*this)(cell, number)) {
							new_extra[number - 1] = true;
						}
					}
					size_t extra_size = 0;
					for (bool b: new_extra) if (b) {
						++extra_size;
					}
					bool normal = cell_count[cell] == 2 && (*this)(cell, loop.var[0]) && (*this)(cell, loop.var[1]);
					if (normal || extra_size == 1 || allowed > 0) {
						loop.cell.push_back(cell);
						loop.set[cell] = true;
						for (size_t r: GetRegion(cell)) {
							++loop.region_use[r];
						}
						FindUniqueLoop(results, loop, !normal ? allowed - 1 : allowed, new_extra, i);
					}
				}
			}
		}
	}
	for (size_t region: GetRegion(next)) {
		--loop.region_use[region];
	}
	loop.set[next] = false;
	loop.cell.pop_back();
}

void Unique::FindHiddenLoop(vector<Loop> &results, Loop &loop, size_t odd_count, size_t even_count, int last_region_type) const { 
	size_t next = loop.cell.back();
	for (int i = 0; i < 3; ++i) if (i != last_region_type) {
		size_t region = GetRegion(next)[i];
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
					loop.type = GetHiddenType(loop);
					if (loop.type > 0) {
						results.push_back(loop);
					}
				}
			} else if (!loop.set[cell] && (*this)(cell, loop.var[0])) {
				bool ok = true;
				for (size_t new_region: GetRegion(cell)) {
					const vector<size_t> &region_count = RegionCount(new_region);
					if (region_count[loop.var[0] - 1] != 2 || region_count[loop.var[1] - 1] == 0) {
						ok = false;
						break;
					}
				}
				if (!ok) {
					continue;
				}
				for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
					if ((*this)(cell, number)) {
						++(loop.cell.size() % 2 == 0 ? odd_count : even_count);
						break;
					}
				}
				if (odd_count <= 1 || even_count <= 1) {
					loop.cell.push_back(cell);
					loop.set[cell] = true;
					FindHiddenLoop(results, loop, odd_count, even_count, i);
				}
			}
		}
	}
	loop.set[loop.cell.back()] = false;
	loop.cell.pop_back();
}

int Unique::GetUniqueType(const Loop &loop) const {
	vector<bool> odd(3 * size), even(3 * size);
	bool is_odd = false;
	for (size_t cell: loop.cell) {
		for (size_t region: GetRegion(cell)) {
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
		if (cell_count[cell] > 2 || !(*this)(cell, loop.var[0]) || !(*this)(cell, loop.var[1])) {
			extra_cell.push_back(cell);
		}
	}
	if (extra_cell.size() == 1) {
		return 1;
	} else {
		size_t count = 0;
		for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
			if (((*this)(extra_cell[0], number) || (*this)(extra_cell[1], number))) {
				++count;
			}
		}
		return count == 1 ? 2 : 3;
	}
}

int Unique::GetHiddenType(const Loop &loop) const {
	vector<bool> odd(3 * size), even(3 * size);
	bool is_odd = false;
	for (size_t cell: loop.cell) {
		for (size_t region: GetRegion(cell)) {
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

	is_odd = false;
	size_t count[2] = {0, 0};
	for (size_t cell: loop.cell) {
		bool ok = false;
		for (size_t number = 1; number <= size; ++number) if (number != loop.var[0] && number != loop.var[1]) {
			if ((*this)(cell, number)) {
				ok = true;
				break;
			}
		}
		if (ok) {
			++count[is_odd ? 1 : 0];
		}
		is_odd = !is_odd;
	}

	if (count[0] == 0) {
		return 1;
	} else if (count[1] == 0) {
		return 2;
	} else if (count[0] == 1) {
		if (count[1] == 1) {
			return 3;
		} else {
			return 4;
		}
	} else {
		return 5;
	}
}
