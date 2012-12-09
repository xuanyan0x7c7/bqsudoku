#pragma once
#include <string>
#include <utility>
#include <vector>
#include "SudokuBase.h"


class Candidate;

class Technique: protected SudokuBase {
protected:
	std::vector<size_t> &board;
	const std::vector<bool> &given;
protected:
	std::vector<std::vector<bool>> &candidate;
	std::vector<std::vector<size_t>> &row_count, &column_count, &box_count;
	std::vector<size_t> &cell_count;
	std::vector<std::vector<size_t>> &row_index, &column_index, &box_index;
	std::vector<std::vector<bool>> &row_contain, &column_contain, &box_contain;
	std::vector<std::vector<bool>> &weak_chain;
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
	inline void Fill(std::size_t, std::size_t);
	void Fill(std::size_t, std::size_t, std::size_t);
	inline void Remove(std::size_t);
	inline void Remove(std::size_t, std::size_t);
	void Remove(std::size_t, std::size_t, std::size_t);
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

inline void Technique::Fill(size_t index, size_t number) {
	Fill(index / size, index % size, number);
}

inline void Technique::Remove(size_t number) {
	Remove(number / (size * size), number / size % size, number % size + 1);
}

inline void Technique::Remove(size_t index, size_t number) {
	Remove(index / size, index % size, number);
}
