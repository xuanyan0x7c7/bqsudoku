#pragma once
#include <string>
#include <utility>
#include <vector>
#include "SudokuBase.h"


class Candidate;

class Technique: protected SudokuBase {
protected:
	std::vector<std::size_t> &board;
	const std::vector<bool> &given;
protected:
	std::vector<std::vector<bool>> &candidate;
	std::vector<std::vector<std::size_t>> &row_count, &column_count, &box_count;
	std::vector<std::size_t> &cell_count;
	std::vector<std::vector<std::size_t>> &row_index, &column_index, &box_index;
	const std::vector<std::vector<bool>> &row_contain, &column_contain, &box_contain;
	std::vector<std::size_t> row_blank, column_blank, box_blank;
	const std::vector<std::vector<bool>> &weak_chain;
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

inline std::vector<bool>::reference Technique::operator ()(std::size_t index, std::size_t number) {
	return candidate[index][number - 1];
}

inline std::vector<bool>::reference Technique::operator ()(std::size_t row, std::size_t column, std::size_t number) {
	return candidate[row * size + column][number - 1];
}

inline bool Technique::operator ()(std::size_t number) const {
	return candidate[number / size][number % size];
}

inline bool Technique::operator ()(std::size_t index, std::size_t number) const {
	return candidate[index][number - 1];
}

inline bool Technique::operator ()(std::size_t row, std::size_t column, std::size_t number) const {
	return candidate[number / size][number % size];
}

inline void Technique::Fill(std::size_t index, std::size_t number) {
	Fill(index / size, index % size, number);
}

inline void Technique::Remove(std::size_t number) {
	Remove(number / (size * size), number / size % size, number % size + 1);
}

inline void Technique::Remove(std::size_t index, std::size_t number) {
	Remove(index / size, index % size, number);
}

inline std::vector<size_t> Technique::CommonEffectCell(size_t n1, size_t n2) const {
	return CommonEffectCell(n1 / (size * size), n1 / size % size, n1 % size + 1, n2 / (size * size), n2 / size % size, n2 % size + 1);
}
