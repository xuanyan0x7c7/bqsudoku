#pragma once
#include <string>
#include <utility>
#include <vector>
#include "SudokuBase.h"


class Candidate;

class Technique: protected SudokuBase {
protected:
	std::vector<std::size_t> &grid;
	const std::vector<bool> &given;
protected:
	std::vector<std::vector<bool>> &candidate;
	std::vector<std::vector<std::size_t>> &row_count, &column_count, &box_count;
	std::vector<std::size_t> &cell_count;
	std::vector<std::vector<std::size_t>> &row_index, &column_index, &box_index;
	const std::vector<std::vector<bool>> &row_contain, &column_contain, &box_contain;
	std::vector<std::size_t> &row_blank, &column_blank, &box_blank;
	const std::vector<std::vector<bool>> &weak_link;
	int &difficulty;
public:
	Technique(const Technique&);
	Technique(Technique&&);
	Technique& operator =(const Technique&);
	Technique& operator =(Technique&&);
	virtual ~Technique();
public:
	Technique(Candidate&);
protected:
	inline std::vector<bool>::reference operator ()(std::size_t);
	inline std::vector<bool>::reference operator ()(std::size_t, std::size_t);
	inline std::vector<bool>::reference operator ()(std::size_t, std::size_t, std::size_t);
	inline bool operator ()(std::size_t) const;
	inline bool operator ()(std::size_t, std::size_t) const;
	inline bool operator ()(std::size_t, std::size_t, std::size_t) const;
	inline void Fill(std::size_t, std::size_t);
	void Fill(std::size_t, std::size_t, std::size_t);
	inline void Remove(std::size_t);
	inline void Remove(std::size_t, std::size_t);
	void Remove(std::size_t, std::size_t, std::size_t);
	inline std::vector<std::size_t> GetRegion(std::size_t) const;
	inline std::vector<std::size_t> RegionCount(std::size_t) const;
	inline std::vector<std::size_t> RegionIndex(std::size_t) const;
	inline std::vector<bool> RegionContain(std::size_t) const;
	inline std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t) const;
public:
	typedef std::pair<std::string, bool> HintType;
	virtual HintType GetHint() = 0;
};

inline std::vector<bool>::reference Technique::operator ()(std::size_t number) {
	return candidate[number / size][number % size];
}

inline std::vector<bool>::reference Technique::operator ()(std::size_t cell, std::size_t number) {
	return candidate[cell][number - 1];
}

inline std::vector<bool>::reference Technique::operator ()(std::size_t row, std::size_t column, std::size_t number) {
	return candidate[row * size + column][number - 1];
}

inline bool Technique::operator ()(std::size_t number) const {
	return candidate[number / size][number % size];
}

inline bool Technique::operator ()(std::size_t cell, std::size_t number) const {
	return candidate[cell][number - 1];
}

inline bool Technique::operator ()(std::size_t row, std::size_t column, std::size_t number) const {
	return candidate[number / size][number % size];
}

inline void Technique::Fill(std::size_t cell, std::size_t number) {
	Fill(cell / size, cell % size, number);
}

inline void Technique::Remove(std::size_t number) {
	Remove(number / (size * size), number / size % size, number % size + 1);
}

inline void Technique::Remove(std::size_t cell, std::size_t number) {
	Remove(cell / size, cell % size, number);
}

inline std::vector<std::size_t> Technique::GetRegion(std::size_t cell) const {
	std::size_t row = cell / size;
	std::size_t column = cell % size;
	return {row / m * m + column / n, size + row, 2 * size + column};
}

inline std::vector<std::size_t> Technique::RegionCount(std::size_t region) const {
	if (region < size) {
		return box_count[region];
	} else if (region < 2 * size) {
		return row_count[region - size];
	} else {
		return column_count[region - 2 * size];
	}
}

inline std::vector<std::size_t> Technique::RegionIndex(std::size_t region) const {
	if (region < size) {
		return box_index[region];
	} else if (region < 2 * size) {
		return row_index[region - size];
	} else {
		return column_index[region - 2 * size];
	}
}

inline std::vector<bool> Technique::RegionContain(std::size_t region) const {
	if (region < size) {
		return box_contain[region];
	} else if (region < 2 * size) {
		return row_contain[region - size];
	} else {
		return column_contain[region - 2 * size];
	}
}

inline std::vector<size_t> Technique::CommonEffectCell(size_t n1, size_t n2) const {
	return CommonEffectCell(n1 / (size * size), n1 / size % size, n1 % size + 1, n2 / (size * size), n2 / size % size, n2 % size + 1);
}
