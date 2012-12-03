#pragma once
#include <ostream>
#include <string>
#include <vector>
#include "Sudoku.h"


class Candidate: public Sudoku {
private:
	std::vector<std::vector<bool>> candidate;
	std::vector<std::vector<size_t>> row_count, column_count, box_count;
	std::vector<size_t> cell_count;
	std::vector<std::vector<size_t>> row_index, column_index, box_index;
	std::vector<std::vector<bool>> row_contain, column_contain, box_contain;
	int difficulty;
public:
	Candidate(const Candidate&);
	Candidate(Candidate&&);
	Candidate& operator=(const Candidate&);
	Candidate& operator=(Candidate&&);
	~Candidate();
public:
	Candidate(const Sudoku&);
private:
	static constexpr int ChainType_XChain = 0;
	static constexpr int ChainType_XYChain = 1;
	static constexpr int ChainType_AIC = 2;
private:
	inline std::vector<bool>::reference operator()(std::size_t);
	inline std::vector<bool>::reference operator()(std::size_t, std::size_t);
	inline std::vector<bool>::reference operator()(std::size_t, std::size_t, std::size_t);
	inline bool operator()(std::size_t) const;
	inline bool operator()(std::size_t, std::size_t) const;
	inline bool operator()(std::size_t, std::size_t, std::size_t) const;
	void Fill(std::size_t, std::size_t);
	void Fill(std::size_t, std::size_t, std::size_t);
	void Remove(std::size_t);
	void Remove(std::size_t, std::size_t);
	void Remove(std::size_t, std::size_t, std::size_t);
private:
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t);
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t);
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t);
	bool IsWeakChain(std::size_t, std::size_t);
	bool IsWeakChain(std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t);
	int ChainType(const std::vector<size_t>&) const;
	void PrintChain(std::ostream&, const std::vector<size_t>&);
public:
	std::string FindNext();
	inline int Difficulty() const;
private:
	std::string Single();
	std::string NakedSingle();
	std::string HiddenSingle();
	std::string LockedCandidate();
	std::string NakedPair(std::size_t);
	std::string HiddenPair(std::size_t);
	std::string Fish(std::size_t);
	std::string Skyscraper();
	std::string _2StringKite();
	std::string TurbotFish();
	std::string ForcingChain();
};

inline std::vector<bool>::reference Candidate::operator()(std::size_t number) {
	return candidate[number / size][number % size];
}

inline std::vector<bool>::reference Candidate::operator()(std::size_t index, std::size_t number) {
	return candidate[index][number - 1];
}

inline std::vector<bool>::reference Candidate::operator()(std::size_t row, std::size_t column, std::size_t number) {
	return candidate[row * size + column][number - 1];
}

inline bool Candidate::operator()(std::size_t number) const {
	return candidate[number / size][number % size];
}

inline bool Candidate::operator()(std::size_t index, std::size_t number) const {
	return candidate[index][number - 1];
}

inline bool Candidate::operator()(std::size_t row, std::size_t column, std::size_t number) const {
	return candidate[row * size + column][number - 1];
}

inline int Candidate::Difficulty() const {
	return difficulty;
}
