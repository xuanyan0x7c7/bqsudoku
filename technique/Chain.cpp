#include <deque>
#include <functional>
#include <sstream>
#include <unordered_map>
#include "Chain.h"
using std::deque;
using std::function;
using std::get;
using std::hash;
using std::make_pair;
using std::make_tuple;
using std::mem_fn;
using std::move;
using std::ostringstream;
using std::pair;
using std::size_t;
using std::string;
using std::tuple;
using std::unordered_map;
using std::vector;


Chain::Chain(const Chain&) = default;
Chain::Chain(Chain&&) = default;
Chain& Chain::operator =(const Chain&) = delete;
Chain& Chain::operator =(Chain&&) = delete;
Chain::~Chain() = default;

Chain::Chain(Candidate &sudoku): Technique(sudoku) {}

namespace std {
	static constexpr size_t seed = 13131;

	template <> struct hash<pair<size_t, size_t>> {
		size_t operator ()(const pair<size_t, size_t> &p) const {
			return p.first * seed + p.second;
		}
	};

	template <> struct hash<pair<Chain::Group, Chain::Group>> {
		size_t operator ()(const pair<vector<size_t>, vector<size_t>> &p) const {
			size_t h = 0;
			for (size_t n: p.first) {
				h = h * seed + n;
			}
			for (size_t n: p.second) {
				h = h * seed + n;
			}
			return h;
		}
	};
}

Technique::HintType Chain::GetHint() {
	static const function<HintType(Chain&)> algs[4]
		= {mem_fn(&Chain::Skyscraper), mem_fn(&Chain::_2StringKite), mem_fn(&Chain::TurbotFish), mem_fn(&Chain::ForcingChain)};
	for (const auto &alg: algs) {
		HintType hint = alg(*this);
		if (!hint.first.empty()) {
			return hint;
		}
	}
	return make_pair("", false);
}

vector<size_t> Chain::CommonEffectCell(const Group &g1, const Group &g2) const {
	vector<size_t> final_vec;
	vector<size_t> vec = CommonEffectCell(g1.front(), g2.front());
	for (size_t cell: vec) {
		bool ok = true;
		for (size_t c: g1) {
			if (!weak_link[cell][c]) {
				ok = false;
				break;
			}
		}
		if (!ok) {
			continue;
		}
		for (size_t c: g2) {
			if (!weak_link[cell][c]) {
				ok = false;
				break;
			}
		}
		if (ok) {
			final_vec.push_back(cell);
		}
	}
	return final_vec;
}

bool Chain::IsWeakChain(const Group &g1, const Group &g2) const {
	for (size_t c1: g1) for (size_t c2: g2) {
		if (!weak_link[c1][c2]) {
			return false;
		}
	}
	return true;
}

Chain::ChainType Chain::GetChainType(const vector<Group> &chain) const {
	bool is_grouped = false;
	for (const Group &group: chain) {
		if (group.size() > 1) {
			is_grouped = true;
			break;
		}
	}
	bool is_xchain = true;
	for (const Group &group: chain) {
		if (group.front() % size != chain.front().front() % size) {
			is_xchain = false;
			break;
		}
	}
	bool is_niceloop = chain.front() == chain.back();
	if (is_grouped) {
		return is_niceloop ? (is_xchain ? ChainType::GroupedNiceXLoop : ChainType::GroupedNiceLoop)
			: (is_xchain ? ChainType::GroupedXChain : ChainType::GroupedAIC);
	} else {
		if (is_xchain) {
			return is_niceloop ? ChainType::XChain : ChainType::NiceXLoop;
		}
		bool is_xychain = true;
		for (auto iter = chain.cbegin(); iter != chain.cend(); iter += 2) {
			if (iter->front() / size != (iter + 1)->front() / size) {
				is_xychain = false;
				break;
			}
		}
		return is_xychain ? ChainType::XYChain : (is_niceloop ? ChainType::NiceLoop : ChainType::AIC);
	}
}

string Chain::Chain2String(const vector<Group> &chain) {
	ostringstream ostr;
	size_t number;
	bool is_strong_link = true;
	switch (GetChainType(chain)) {
		case ChainType::XChain:
			difficulty += 1000 + 100 * chain.size();
			ostr << "X-Chain (";
			number = chain.front().front() % size + 1;
			ostr << Number2String(number) << " of ";
			for (const Group &group: chain) {
				size_t num = group.front();
				is_strong_link = !is_strong_link;
				if (num != chain.front().front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				ostr << Cell2String(num / size);
			}
			ostr << "):";
			break;
		case ChainType::NiceXLoop: 
			difficulty += 800 + 100 * chain.size();
			ostr << "Nice X-Loop (";
			number = chain.front().front() % size + 1;
			ostr << Number2String(number) << " of ";
			for (const Group &group: chain) {
				size_t num = group.front();
				is_strong_link = !is_strong_link;
				if (num != chain.front().front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				ostr << Cell2String(num / size);
			}
			ostr << "):";
			break;
		case ChainType::XYChain:
			difficulty += 1000 + 50 * chain.size();
			ostr << "XY-" << (chain.size() == 6 ? "Wing" : "Chain") << " (";
			for (auto iter = chain.cbegin(); iter != chain.cend(); ++iter) {
				if (iter != chain.cbegin()) {
					ostr << "--";
				}
				size_t num = iter->front();
				ostr << Cell2String(num / size) << '(' << Number2String(num % size + 1) << "==";
				ostr << Number2String((++iter)->front() % size + 1) << ')';
			}
			ostr << "):";
			break;
		case ChainType::AIC:
			difficulty += 2000 + 200 * chain.size();
			if (chain.front().front() % size != chain.back().front() % size) {
				difficulty += 500;
			}
			ostr << "AIC (";
			for (const Group &group: chain) {
				size_t num = group.front();
				is_strong_link = !is_strong_link;
				if (num != chain.front().front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				ostr << Cell2String(num / size) << '(' << Number2String(num % size + 1) << ')';
			}
			ostr << "):";
			break;
		case ChainType::NiceLoop:
			difficulty += 1800 + 200 * chain.size();
			ostr << "Nice Loop (";
			for (const Group &group: chain) {
				size_t num = group.front();
				is_strong_link = !is_strong_link;
				if (num != chain.front().front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				ostr << Cell2String(num / size) << '(' << Number2String(num % size + 1) << ')';
			}
			ostr << "):";
			break;
		case ChainType::GroupedXChain:
			difficulty += 1500 + 150 * chain.size();
			ostr << "Grouped X-Chain (";
			number = chain.front().front() % size + 1;
			ostr << Number2String(number) << " of ";
			for (const Group &group: chain) {
				is_strong_link = !is_strong_link;
				if (group.front() != chain.front().front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				if (group.size() == 1) {
					ostr << Row2String(group.front() / (size * size)) << Column2String(group.front() / size % size);
				} else if (group.front() / (size * size) == group.back() / (size * size)) {
					ostr << Row2String(group.front() / (size * size));
					for (size_t num: group) {
						ostr << Column2String(num / size % size);
					}
				} else {
					for (size_t num: group) {
						ostr << Row2String(num / (size * size));
					}
					ostr << Column2String(group.front() / size % size);
				}
			}
			ostr << "):";
			break;
		case ChainType::GroupedNiceXLoop:
			difficulty += 1300 + 150 * chain.size();
			ostr << "Grouped Nice X-Loop (";
			number = chain.front().front() % size + 1;
			ostr << Number2String(number) << " of ";
			for (const Group &group: chain) {
				is_strong_link = !is_strong_link;
				if (group.front() != chain.front().front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				if (group.size() == 1) {
					ostr << Row2String(group.front() / (size * size)) << Column2String(group.front() / size % size);
				} else if (group.front() / (size * size) == group.back() / (size * size)) {
					ostr << Row2String(group.front() / (size * size));
					for (size_t num: group) {
						ostr << Column2String(num / size % size);
					}
				} else {
					for (size_t num: group) {
						ostr << Row2String(num / (size * size));
					}
					ostr << Column2String(group.front() / size % size);
				}
			}
			ostr << "):";
			break;
		case ChainType::GroupedAIC:
			difficulty += 2000 + 300 * chain.size();
			if (chain.front().front() % size != chain.back().front() % size) {
				difficulty += 500;
			}
			ostr << "Grouped AIC (";
			for (const Group &group: chain) {
				is_strong_link = !is_strong_link;
				if (group != chain.front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				if (group.size() == 1) {
					ostr << Row2String(group.front() / (size * size)) << Column2String(group.front() / size % size);
				} else if (group.front() / (size * size) == group.back() / (size * size)) {
					ostr << Row2String(group.front() / (size * size));
					for (size_t num: group) {
						ostr << Column2String(num / size % size);
					}
				} else {
					for (size_t num: group) {
						ostr << Row2String(num / (size * size));
					}
					ostr << Column2String(group.front() / size % size);
				}
				ostr << '(' << Number2String(group.front() % size + 1) << ')';
			}
			ostr << "):";
			break;
		case ChainType::GroupedNiceLoop:
			difficulty += 1800 + 300 * chain.size();
			ostr << "Grouped Nice Loop (";
			for (const Group &group: chain) {
				is_strong_link = !is_strong_link;
				if (group != chain.front()) {
					ostr << (is_strong_link ? "==" : "--");
				}
				if (group.size() == 1) {
					ostr << Row2String(group.front() / (size * size)) << Column2String(group.front() / size % size);
				} else if (group.front() / (size * size) == group.back() / (size * size)) {
					ostr << Row2String(group.front() / (size * size));
					for (size_t num: group) {
						ostr << Column2String(num / size % size);
					}
				} else {
					for (size_t num: group) {
						ostr << Row2String(num / (size * size));
					}
					ostr << Column2String(group.front() / size % size);
				}
				ostr << '(' << Number2String(group.front() % size + 1) << ')';
			}
			ostr << "):";
			break;
		default:
			break;
	}

	return ostr.str();
}

string Chain::Chain2String(const vector<Group> &chain, int) {
	ostringstream ostr;
	bool is_strong_link = true;
	for (const Group &group: chain) {
		is_strong_link = !is_strong_link;
		if (group != chain.front()) {
			ostr << (is_strong_link ? "==" : "--");
		}
		if (group.size() == 1) {
			ostr << Row2String(group.front() / (size * size)) << Column2String(group.front() / size % size);
		} else if (group.front() / (size * size) == group.back() / (size * size)) {
			ostr << Row2String(group.front() / (size * size));
			for (size_t num: group) {
				ostr << Column2String(num / size % size);
			}
		} else {
			for (size_t num: group) {
				ostr << Row2String(num / (size * size));
			}
			ostr << Column2String(group.front() / size % size);
		}
		ostr << '(' << Number2String(group.front() % size + 1) << ')';
	}
	return ostr.str();
}

Technique::HintType Chain::Skyscraper() {
	for (size_t number = 1; number <= size; ++number) {
		vector<pair<size_t, size_t>> rows;
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row][number - 1] == 2) {
				size_t c = 0;
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						c = c * size + column;
					}
				}
				rows.push_back(make_pair(row, c));
			}
		}
		size_t count = rows.size();
		if (count == 0) {
			continue;
		}
		for (size_t r1 = 0; r1 < count - 1; ++r1) {
			size_t row1 = rows[r1].first;
			size_t column11 = rows[r1].second / size;
			size_t column12 = rows[r1].second % size;
			for (size_t r2 = r1 + 1; r2 < count; ++r2) {
				size_t row2 = rows[r2].first;
				if (row1 / m != row2 / m) {
					size_t column21 = rows[r2].second / size;
					size_t column22 = rows[r2].second % size;
					vector<size_t> elim;
					if (column11 == column21) {
						for (size_t cell: CommonEffectCell(row1, column12, row2, column22)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					} else if (column12 == column22) {
						for (size_t cell: CommonEffectCell(row1, column11, row2, column21)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					}
					if (!elim.empty()) {
						difficulty += 1500;
						ostringstream ostr;
						ostr << "Skyscraper (" << Number2String(number) << " in Row " << Row2String(row1) << Row2String(row2) << "):";
						for (size_t cell: elim) {
							Remove(cell, number);
							ostr << ' ' << Cell2String(cell) << "!=" << Number2String(number);
						}
						return make_pair(ostr.str(), false);
					}
				}
			}
		}

		vector<pair<size_t, size_t>> columns;
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column][number - 1] == 2) {
				size_t r = 0;
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						r = r * size + row;
					}
				}
				columns.push_back(make_pair(column, r));
			}
		}
		count = columns.size();
		if (count == 0) {
			continue;
		}
		for (size_t c1 = 0; c1 < count - 1; ++c1) {
			size_t column1 = columns[c1].first;
			size_t row11 = columns[c1].second / size;
			size_t row12 = columns[c1].second % size;
			for (size_t c2 = c1 + 1; c2 < count; ++c2) {
				size_t column2 = columns[c2].first;
				if (column1 / n != column2 / n) {
					size_t row21 = columns[c2].second / size;
					size_t row22 = columns[c2].second % size;
					vector<size_t> elim;
					if (row11 == row21) {
						for (size_t cell: CommonEffectCell(row12, column1, row22, column2)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					} else if (row12 == row22) {
						for (size_t cell: CommonEffectCell(row11, column1, row21, column2)) {
							if ((*this)(cell, number)) {
								elim.push_back(cell);
							}
						}
					}
					if (!elim.empty()) {
						difficulty += 1500;
						ostringstream ostr;
						ostr << "Skyscraper (" << Number2String(number) << " in Column " << Column2String(column1) << Column2String(column2) << "):";
						for (size_t cell: elim) {
							Remove(cell, number);
							ostr << ' ' << Cell2String(cell) << "!=" << Number2String(number);
						}
						return make_pair(ostr.str(), false);
					}
				}
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Chain::_2StringKite() {
	for (size_t number = 1; number <= size; ++number) {
		vector<pair<size_t, size_t>> rows, columns;
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row][number - 1] == 2) {
				size_t c = 0;
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						c = c * size + column;
					}
				}
				rows.push_back(make_pair(row, c));
			}
		}
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column][number - 1] == 2) {
				size_t r = 0;
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						r = r * size + row;
					}
				}
				columns.push_back(make_pair(column, r));
			}
		}
		size_t rcount = rows.size();
		size_t ccount = columns.size();
		for (size_t r = 0; r < rcount; ++r) {
			size_t row1 = rows[r].first;
			size_t column11 = rows[r].second / size;
			size_t column12 = rows[r].second % size;
			for (size_t c = 0; c < ccount; ++c) {
				size_t column2 = columns[c].first;
				size_t row21 = columns[c].second / size;
				size_t row22 = columns[c].second % size;
				if (row1 / m == row21 / m && row1 != row21) {
					if (column2 / n == column11 / n && column2 != column11) {
						if ((*this)(row22, column12, number)) {
							Remove(row22, column12, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2String(number) << " in Row " << Row2String(row1) << " Column " << Column2String(column2)
								<< "): " << Cell2String(row22 * size + column12) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					} else if (column2 / n == column12 / n
						&& column2 != column12) {
						if ((*this)(row22, column11, number)) {
							Remove(row22, column11, number);
							difficulty += 0x2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2String(number) << " in Row " << Row2String(row1) << " Column " << Column2String(column2)
								<< "): " << Cell2String(row22 * size + column11) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (row1 / m == row22 / m && row1 != row22) {
					if (column2 / n == column11 / n && column2 != column11) {
						if ((*this)(row21, column12, number)) {
							Remove(row21, column12, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2String(number) << " in Row " << Row2String(row1) << " Column " << Column2String(column2)
								<< "): " << Cell2String(row21 * size + column12) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					} else if (column2 / n == column12 / n
						&& column2 != column12) {
						if ((*this)(row21, column11, number)) {
							Remove(row21, column11, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "2 String Kite (" << Number2String(number) << " in Row " << Row2String(row1) << " Column " << Column2String(column2)
								<< "): " << Cell2String(row21 * size + column11) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				}
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Chain::TurbotFish() {
	for (size_t number = 1; number <= size; ++number) {
		vector<pair<size_t, size_t>> boxes, rows, columns;
		for (size_t box = 0; box < size; ++box) {
			if (box_count[box][number - 1] == 2) {
				size_t c = 0;
				for (size_t cell: box_index[box]) {
					if ((*this)(cell, number)) {
						c = c * size * size + cell;
					}
				}
				boxes.push_back(make_pair(box, c));
			}
		}
		for (size_t row = 0; row < size; ++row) {
			if (row_count[row][number - 1] == 2) {
				size_t c = 0;
				for (size_t column = 0; column < size; ++column) {
					if ((*this)(row, column, number)) {
						c = c * size + column;
					}
				}
				rows.push_back(make_pair(row, c));
			}
		}
		for (size_t column = 0; column < size; ++column) {
			if (column_count[column][number - 1] == 2) {
				size_t r = 0;
				for (size_t row = 0; row < size; ++row) {
					if ((*this)(row, column, number)) {
						r = r * size + row;
					}
				}
				columns.push_back(make_pair(column, r));
			}
		}
		size_t bcount = boxes.size();
		size_t rcount = rows.size();
		size_t ccount = columns.size();
		for (size_t b = 0; b < bcount; ++b) {
			size_t row11 = boxes[b].second / (size * size) / size;
			size_t column11 = boxes[b].second / (size * size) % size;
			size_t row12 = boxes[b].second % (size * size) / size;
			size_t column12 = boxes[b].second % (size * size) % size;
			for (size_t r = 0; r < rcount; ++r) {
				size_t row2 = rows[r].first;
				size_t column21 = rows[r].second / size;
				size_t column22 = rows[r].second % size;
				if (column11 == column21) {
					if (row11 / m != row2 / m && column12 != column22) {
						if ((*this)(row12, column22, number)) {
							Remove(row12, column22, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish ("
								<< Number2String(number) << " in Box " << boxes[b].first + 1 << " and Row " << Row2String(row2) << "): "
								<< Cell2String(row12 * size + column22) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (column11 == column22) {
					if (row11 / m != row2 / m && column12 != column21) {
						if ((*this)(row12, column21, number)) {
							Remove(row12, column21, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish ("
								<< Number2String(number) << " in Box " << boxes[b].first + 1 << " and Row " << Row2String(row2) << "): "
								<< Cell2String(row12 * size + column21) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (column12 == column21) {
					if (row12 / m != row2 / m && column11 != column22) {
						if ((*this)(row11, column22, number)) {
							Remove(row11, column22, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish ("
								<< Number2String(number) << " in Box " << boxes[b].first + 1 << " and Row " << Row2String(row2) << "): "
								<< Cell2String(row11 * size + column22) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (column12 == column22) {
					if (row12 / m != row2 / m && column11 != column21) {
						if ((*this)(row11, column21, number)) {
							Remove(row11, column21, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish ("
								<< Number2String(number) << " in Box " << boxes[b].first + 1 << " and Row " << Row2String(row2) << "): "
								<< Cell2String(row11 * size + column21) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				}
			}
			for (size_t c = 0; c < ccount; ++c) {
				size_t column2 = columns[c].first;
				size_t row21 = columns[c].second / size;
				size_t row22 = columns[c].second % size;
				if (row11 == row21) {
					if (column11 / n != column2 / n && row12 != row22) {
						if ((*this)(row22, column12, number)) {
							Remove(row22, column12, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2String(number)
								<< " in Box " << boxes[b].first + 1 << " and Column " << Column2String(column2) << "): "
								<< Cell2String(row22 * size + column12) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (row11 == row22) {
					if (column11 / n != column2 / n && row12 != row21) {
						if ((*this)(row21, column12, number)) {
							Remove(row21, column12, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2String(number)
								<< " in Box " << boxes[b].first + 1 << " and Column " << Column2String(column2) << "): "
								<< Cell2String(row21 * size + column12) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (row12 == row21) {
					if (column12 / n != column2 / n && row11 != row22) {
						if ((*this)(row22, column11, number)) {
							Remove(row22, column11, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2String(number)
								<< " in Box " << boxes[b].first + 1 << " and Column " << Column2String(column2) << "): "
								<< Cell2String(row22 * size + column11) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				} else if (row12 == row22) {
					if (column12 / n != column2 / n && row11 != row21) {
						if ((*this)(row21, column11, number)) {
							Remove(row21, column11, number);
							difficulty += 2000;
							ostringstream ostr;
							ostr << "Turbot Fish (" << Number2String(number)
								<< " in Box " << boxes[b].first << " and Column " << Column2String(column2) << "): "
								<< Cell2String(row21 * size + column11) << "!=" << Number2String(number);
							return make_pair(ostr.str(), false);
						}
					}
				}
			}
		}
	}

	return make_pair("", false);
}

Technique::HintType Chain::ForcingChain() {
	deque<unordered_map<pair<Group, Group>, vector<Group>>> strong_link(1);
	auto &predecessor = strong_link.front();
	for (size_t number = 1; number <= size; ++number) {
		for (size_t box = 0; box < size; ++box) {
			vector<size_t> contain;
			for (size_t cell: box_index[box]) {
				if ((*this)(cell, number)) {
					contain.push_back(cell);
				}
			}
			if (box_count[box][number - 1] == 2) {
				size_t c[2], count = 0;
				for (size_t cell: contain) {
					c[count++] = cell * size + number - 1;
				}
				Group g1(1, c[0]), g2(1, c[1]);
				predecessor[make_pair(g1, g2)] = {g1, g2};
				predecessor[make_pair(g2, g1)] = {g2, g1};
			} else if (box_count[box][number - 1] > 0) {
				size_t r = box / m * m;
				size_t c = box % m * n;
				bool no_strong_chain = false;
				for (size_t row = r; row < r + m; ++row) {
					bool ok = false;
					for (size_t cell: contain) {
						if (!row_contain[row][cell]) {
							ok = true;
							break;
						}
					}
					if (!ok) {
						no_strong_chain = true;
						break;
					}
				}
				if (no_strong_chain) {
					continue;
				}
				no_strong_chain = false;
				for (size_t column = c; column < c + n; ++column) {
					bool ok = false;
					for (size_t cell: contain) {
						if (!column_contain[column][cell]) {
							ok = true;
							break;
						}
					}
					if (!ok) {
						no_strong_chain = true;
						break;
					}
				}
				if (no_strong_chain) {
					continue;
				}
				for (size_t r1 = r; r1 < r + m - 1; ++r1) for (size_t r2 = r1 + 1; r2 < r + m; ++r2) {
					bool ok = true;
					for (size_t cell: contain) {
						if (!row_contain[r1][cell] && !row_contain[r2][cell]) {
							ok = false;
							break;
						}
					}
					if (ok) {
						Group g1, g2;
						for (size_t cell: contain) {
							(row_contain[r1][cell] ? g1 : g2).push_back(cell * size + number - 1);
						}
						predecessor[make_pair(g1, g2)] = {g1, g2};
						predecessor[make_pair(g2, g1)] = {g2, g1};
						break;
					}
				}
				for (size_t c1 = c; c1 < c + n - 1; ++c1) for (size_t c2 = c1 + 1; c2 < c + n; ++c2) {
					bool ok = true;
					for (size_t cell: contain) {
						if (!column_contain[c1][cell] && !column_contain[c2][cell]) {
							ok = false;
							break;
						}
					}
					if (ok) {
						Group g1, g2;
						for (size_t cell: contain) {
							(column_contain[c1][cell] ? g1 : g2).push_back(cell * size + number - 1);
						}
						predecessor[make_pair(g1, g2)] = {g1, g2};
						predecessor[make_pair(g2, g1)] = {g2, g1};
						break;
					}
				}
				for (size_t row = r; row < r + m; ++row) for (size_t column = c; column < c + n; ++column) {
					bool ok = true;
					for (size_t cell: contain) {
						if (!row_contain[row][cell] && !column_contain[column][cell]) {
							ok = false;
							break;
						}
					}
					if (ok) {
						Group g1, g2;
						for (size_t cell: contain) {
							(row_contain[row][cell] ? g1 : g2).push_back(cell * size + number - 1);
						}
						if (g1.size() != 1 && g2.size() != 1) {
							predecessor[make_pair(g1, g2)] = {g1, g2};
							predecessor[make_pair(g2, g1)] = {g2, g1};
						}
						g1.clear();
						g2.clear();
						for (size_t cell: contain) {
							(column_contain[column][cell] ? g1 : g2).push_back(cell * size + number - 1);
						}
						if (g1.size() != 1 && g2.size() != 1) {
							predecessor[make_pair(g1, g2)] = {g1, g2};
							predecessor[make_pair(g2, g1)] = {g2, g1};
						}
						break;
					}
				}
			}
		}
		for (size_t row = 0; row < size; ++row) {
			vector<size_t> contain;
			for (size_t cell: row_index[row]) {
				if ((*this)(cell, number)) {
					contain.push_back(cell);
				}
			}
			if (row_count[row][number - 1] == 2) {
				size_t c[2], count = 0;
				for (size_t cell: contain) {
					c[count++] = cell * size + number - 1;
				}
				Group g1(1, c[0]), g2(1, c[1]);
				predecessor[make_pair(g1, g2)] = {g1, g2};
				predecessor[make_pair(g2, g1)] = {g2, g1};
			} else if (row_count[row][number - 1] > 0) {
				vector<size_t> boxes;
				for (size_t i = 0; i < m; ++i) {
					boxes.push_back(row / m * m + i);
				}
				bool no_strong_chain = false;
				for (size_t box: boxes) {
					bool ok = false;
					for (size_t cell: contain) {
						if (!box_contain[box][cell]) {
							ok = true;
							break;
						}
					}
					if (!ok) {
						no_strong_chain = true;
						break;
					}
				}
				if (no_strong_chain) {
					continue;
				}
				for (size_t b1 = 0; b1 < m - 1; ++b1) for (size_t b2 = b1 + 1; b2 < m; ++b2) {
					bool ok = true;
					for (size_t cell: contain) {
						if (!box_contain[boxes[b1]][cell] && !box_contain[boxes[b2]][cell]) {
							ok = false;
							break;
						}
					}
					if (ok) {
						Group g1, g2;
						for (size_t cell: contain) {
							(box_contain[boxes[b1]][cell] ? g1 : g2).push_back(cell * size + number - 1);
						}
						predecessor[make_pair(g1, g2)] = {g1, g2};
						predecessor[make_pair(g2, g1)] = {g2, g1};
						break;
					}
				}
			}
		}
		for (size_t column = 0; column < size; ++column) {
			vector<size_t> contain;
			for (size_t cell: column_index[column]) {
				if ((*this)(cell, number)) {
					contain.push_back(cell);
				}
			}
			if (column_count[column][number - 1] == 2) {
				size_t c[2], count = 0;
				for (size_t cell: contain) {
					c[count++] = cell * size + number - 1;
				}
				Group g1(1, c[0]), g2(1, c[1]);
				predecessor[make_pair(g1, g2)] = {g1, g2};
				predecessor[make_pair(g2, g1)] = {g2, g1};
			} else if (column_count[column][number - 1] > 0) {
				vector<size_t> boxes;
				for (size_t i = 0; i < n; ++i) {
					boxes.push_back(i * m + column / n);
				}
				bool no_strong_chain = false;
				for (size_t box: boxes) {
					bool ok = false;
					for (size_t cell: contain) {
						if (!box_contain[box][cell]) {
							ok = true;
							break;
						}
					}
					if (!ok) {
						no_strong_chain = true;
						break;
					}
				}
				if (no_strong_chain) {
					continue;
				}
				for (size_t b1 = 0; b1 < n - 1; ++b1) for (size_t b2 = b1 + 1; b2 < n; ++b2) {
					bool ok = true;
					for (size_t cell: contain) {
						if (!box_contain[boxes[b1]][cell] && !box_contain[boxes[b2]][cell]) {
							ok = false;
							break;
						}
					}
					if (ok) {
						Group g1, g2;
						for (size_t cell: contain) {
							(box_contain[boxes[b1]][cell] ? g1 : g2).push_back(cell * size + number - 1);
						}
						predecessor[make_pair(g1, g2)] = {g1, g2};
						predecessor[make_pair(g2, g1)] = {g2, g1};
						break;
					}
				}
			}
		}
	}
	for (size_t cell = 0; cell < size * size; ++cell) {
		if (cell_count[cell] == 2) {
			size_t c[2], count = 0;
			for (size_t number = 1; number <= size; ++number) {
				if ((*this)(cell, number)) {
					c[count++] = cell * size + number - 1;
				}
			}
			Group g1(1, c[0]), g2(1, c[1]);
			predecessor[make_pair(g1, g2)] = {g1, g2};
			predecessor[make_pair(g2, g1)] = {g2, g1};
		}
	}

	bool ok = true;
	while (ok) {
		ok = false;
		unordered_map<pair<Group, Group>, vector<Group>> link;
		size_t original_size = strong_link.size();
		for (size_t d = 0; d <= (original_size - 1) / 2; ++d) {
			for (const auto &link1: strong_link[d]) {
				const Group &group11 = link1.first.first;
				const Group &group12 = link1.first.second;
				if (group11 > group12) {
					continue;
				}
				const vector<Group> &l1 = link1.second;
				for (const auto &link2: strong_link[original_size - d - 1]) {
					const Group &group21 = link2.first.first;
					const Group &group22 = link2.first.second;
					if (group21 > group22) {
						continue;
					}
					const vector<Group> &l2 = link2.second;
					if (IsWeakChain(group11, group21)) {
						pair<Group, Group> key1(group12, group22);
						pair<Group, Group> key2(group22, group12);
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size(); ++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							vector<Group> new_vector1(l1.crbegin(), l1.crend());
							new_vector1.insert(new_vector1.end(), l2.cbegin(), l2.cend());
							link[key1] = new_vector1;
							vector<Group> new_vector2(l2.crbegin(), l2.crend());
							new_vector2.insert(new_vector2.end(), l1.cbegin(), l1.cend());
							link[key2] = new_vector2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_vector1;
								(strong_link.back())[key2] = new_vector2;
							}
							vector<size_t> common_effect_cell = CommonEffectCell(group12, group22);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								ostringstream ostr;
								ostr << Chain2String(new_vector1);
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' ' << Cell2String(cell / size) << "!=" << Number2String(cell % size + 1);
								}
								return make_pair(ostr.str(), false);
							}
						}
					}
					if (IsWeakChain(group11, group22)) {
						pair<Group, Group> key1(group12, group21);
						pair<Group, Group> key2(group21, group12);
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size(); ++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							vector<Group> new_vector1(l1.crbegin(), l1.crend());
							new_vector1.insert(new_vector1.end(), l2.crbegin(), l2.crend());
							link[key1] = new_vector1;
							vector<Group> new_vector2 = l2;
							new_vector2.insert(new_vector2.end(), l1.cbegin(), l1.cend());
							link[key2] = new_vector2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_vector1;
								(strong_link.back())[key2] = new_vector2;
							}
							vector<size_t> common_effect_cell = CommonEffectCell(group12, group21);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								ostringstream ostr;
								ostr << Chain2String(new_vector1);
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' ' << Cell2String(cell / size) << "!=" << Number2String(cell % size + 1);
								}
								return make_pair(ostr.str(), false);
							}
						}
					}
					if (IsWeakChain(group12, group21)) {
						pair<Group, Group> key1(group11, group22);
						pair<Group, Group> key2(group22, group11);
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size(); ++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							vector<Group> new_vector1(l1);
							new_vector1.insert(new_vector1.end(), l2.cbegin(), l2.cend());
							link[key1] = new_vector1;
							vector<Group> new_vector2(l2.crbegin(), l2.crend());
							new_vector2.insert(new_vector2.end(), l1.crbegin(), l1.crend());
							link[key2] = new_vector2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_vector1;
								(strong_link.back())[key2] = new_vector2;
							}
							vector<size_t> common_effect_cell = CommonEffectCell(group11, group22);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								ostringstream ostr;
								ostr << Chain2String(new_vector1);
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' ' << Cell2String(cell / size) << "!=" << Number2String(cell % size + 1);
								}
								return make_pair(ostr.str(), false);
							}
						}
					}
					if (IsWeakChain(group12, group22)) {
						pair<Group, Group> key1(group11, group21);
						pair<Group, Group> key2(group21, group11);
						bool found = false;
						for (size_t depth = 0; depth < strong_link.size(); ++depth) {
							const auto &m = strong_link[depth];
							if (m.find(key1) != m.cend()) {
								found = true;
								break;
							}
						}
						if (!found) {
							vector<Group> new_vector1(l1);
							new_vector1.insert(new_vector1.end(), l2.crbegin(), l2.crend());
							link[key1] = new_vector1;
							vector<Group> new_vector2(l2);
							new_vector2.insert(new_vector2.end(), l1.crbegin(), l1.crend());
							link[key2] = new_vector2;
							if (!ok) {
								ok = true;
								strong_link.push_back(link);
							} else {
								(strong_link.back())[key1] = new_vector1;
								(strong_link.back())[key2] = new_vector2;
							}
							vector<size_t> common_effect_cell = CommonEffectCell(group11, group21);
							vector<size_t> elim;
							for (size_t cell: common_effect_cell) {
								if ((*this)(cell)) {
									elim.push_back(cell);
								}
							}
							if (!elim.empty()) {
								ostringstream ostr;
								ostr << Chain2String(new_vector1);
								for (size_t cell: elim) {
									Remove(cell);
									ostr << ' ' << Cell2String(cell / size) << "!=" << Number2String(cell % size + 1);
								}
								return make_pair(ostr.str(), false);
							}
						}
					}
				}
			}
		}
	}

	// TODO output && sort by chain length
	vector<vector<bool>> new_weak_link(weak_link);
	unordered_map<pair<size_t, size_t>, vector<Group>> all_weak_link;
	for (const auto &links: strong_link) for (const auto &link: links) {
		Group g1 = link.first.first;
		Group g2 = link.first.second;
		if (g1 < g2) {
			for (size_t i = 0; i < size * size * size; ++i) if ((*this)(i)) {
				bool ok = true;
				for (size_t cell: g1) if (!weak_link[cell][i]) {
					ok = false;
					break;
				}
				if (!ok) {
					continue;
				}
				for (size_t j = 0; j < size * size * size; ++j) if ((*this)(j)) {
					bool ok = true;
					for (size_t cell: g2) if (!weak_link[cell][j]) {
						ok = false;
						break;
					}
					if (ok) {
						if (!new_weak_link[i][j] || link.second.size() < all_weak_link[{i, j}].size()) {
							all_weak_link[{i, j}] = vector<Group>(link.second);
							all_weak_link[{j, i}] = vector<Group>(link.second.crbegin(), link.second.crend());
							new_weak_link[i][j] = new_weak_link[j][i] = true;
						}
					}
				}
			}
		}
	}

	tuple<string, size_t, int> best_choice("", 0, 0x7fffffff);
	for (size_t cell = 0; cell < size * size; ++cell) if (cell_count[cell] > 2) {
		for (size_t c = 0; c < size * size * size; ++c) if ((*this)(c)) {
			bool ok = true;
			for (size_t number = 1; number <= size; ++number) if ((*this)(cell, number)) {
				if (!new_weak_link[cell * size + number - 1][c]) {
					ok = false;
					break;
				}
			}
			if (ok) {
				int difficulty = 0;
				ostringstream ostr;
				ostr << "Cell Forcing Chains (\n";
				for (size_t number = 1; number <= size; ++number) if ((*this)(cell, number)) {
					const vector<Group> &chain = all_weak_link[{cell * size + number - 1, c}];
					difficulty += 2000 + 200 * chain.size();
					ostr << Cell2String(cell) << '(' << Number2String(number) << ')' << "--";
					if (chain.size() > 0) {
						ostr << Chain2String(chain, 0) << "--";
					}
					ostr << Cell2String(c / size) << '(' << Number2String(c % size + 1) << ")\n";
				}
				ostr << "): " << Cell2String(c / size) << "!=" << Number2String(c % size + 1);
				if (difficulty < get<2>(best_choice)) {
					best_choice = make_tuple(ostr.str(), c, difficulty);
				}
			}
		}
	}
	for (size_t number = 1; number <= size; ++number) {
		for (size_t region = 0; region < 3 * size; ++region) if (RegionCount(region)[number - 1] > 2) {
			for (size_t c = 0; c < size * size * size; ++c) if ((*this)(c)) {
				bool ok = true;
				for (size_t cell: RegionIndex(region)) if ((*this)(cell, number)) {
					if (!new_weak_link[cell * size + number - 1][c]) {
						ok = false;
						break;
					}
				}
				if (ok) {
					int difficulty = 0;
					ostringstream ostr;
					ostr << "Region Forcing Chains (\n";
					for (size_t cell: RegionIndex(region)) if ((*this)(cell, number)) {
						const vector<Group> &chain = all_weak_link[{cell * size + number - 1, c}];
						difficulty += 2000 + 200 * chain.size();
						ostr << Cell2String(cell) << '(' << Number2String(number) << ')' << "--";
						if (chain.size() > 0) {
							ostr << Chain2String(chain, 0) << "--";
						}
						ostr << Cell2String(c / size) << '(' << Number2String(c % size + 1) << ")\n";
					}
					ostr << "): " << Cell2String(c / size) << "!=" << Number2String(c % size + 1);
					if (difficulty < get<2>(best_choice)) {
						best_choice = make_tuple(ostr.str(), c, difficulty);
					}
				}
			}
		}
	}

	if (get<2>(best_choice) != 0x7fffffff) {
		difficulty += get<2>(best_choice);
		Remove(get<1>(best_choice));
		return make_pair(get<0>(best_choice), false);
	} else {
		return make_pair("", false);
	}
}
