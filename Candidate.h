#pragma once
#include <string>
#include <vector>
#include "Candidate.h"
#include "Sudoku.h"
#include "Technique.h"


class Candidate: public Sudoku {
	friend class Technique;
protected:
	std::vector<std::vector<bool>> candidate;
	std::vector<std::vector<std::size_t>> row_count, column_count, box_count;
	std::vector<std::size_t> cell_count;
	std::vector<std::vector<std::size_t>> row_index, column_index, box_index;
	std::vector<std::vector<bool>> row_contain, column_contain, box_contain;
	std::vector<std::size_t> row_blank, column_blank, box_blank;
	std::vector<std::vector<bool>> weak_chain;
	int difficulty;
public:
	Candidate(const Candidate&);
	Candidate(Candidate&&);
	Candidate& operator =(const Candidate&);
	Candidate& operator =(Candidate&&);
	~Candidate();
public:
	Candidate(const Sudoku&);
protected:
	inline std::vector<bool>::reference operator ()(std::size_t, std::size_t);
	inline std::vector<bool>::reference operator ()(std::size_t, std::size_t, std::size_t);
	inline bool operator ()(std::size_t, std::size_t) const;
	inline bool operator ()(std::size_t, std::size_t, std::size_t) const;
	inline void Fill(std::size_t, std::size_t);
	void Fill(std::size_t, std::size_t, std::size_t);
	inline void Remove(std::size_t, std::size_t);
	void Remove(std::size_t, std::size_t, std::size_t);
private:
	std::vector<Technique*> technique;
public:
	virtual Technique::HintType GetHint();
	inline int Difficulty() const;
};

inline std::vector<bool>::reference Candidate::operator ()(std::size_t cell, std::size_t number) {
	return candidate[cell][number - 1];
}

inline std::vector<bool>::reference Candidate::operator ()(std::size_t row, std::size_t column, std::size_t number) {
	return candidate[row * size + column][number - 1];
}

inline bool Candidate::operator ()(std::size_t cell, std::size_t number) const {
	return candidate[cell][number - 1];
}

inline bool Candidate::operator ()(std::size_t row, std::size_t column, std::size_t number) const {
	return candidate[row * size + column][number - 1];
}

inline void Candidate::Fill(std::size_t cell, std::size_t number) {
	Fill(cell / size, cell % size, number);
}

inline void Candidate::Remove(std::size_t cell, std::size_t number) {
	Remove(cell / size, cell % size, number);
}

inline int Candidate::Difficulty() const {
	return difficulty;
}
