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
	static constexpr size_t ChainType_XChain = 0;
	static constexpr size_t ChainType_XYChain = 1;
	static constexpr size_t ChainType_GroupedXChain = 2;
	static constexpr size_t ChainType_AIC = 3;
	static constexpr size_t ChainType_GroupedAIC = 4;
private:
	inline std::vector<bool>::reference operator()(std::size_t);
	inline std::vector<bool>::reference operator()(std::size_t, std::size_t);
	inline std::vector<bool>::reference operator()(std::size_t, std::size_t, std::size_t);
	void Fill(std::size_t, std::size_t);
	void Fill(std::size_t, std::size_t, std::size_t);
	void Remove(std::size_t);
	void Remove(std::size_t, std::size_t);
	void Remove(std::size_t, std::size_t, std::size_t);
private:
	typedef std::vector<std::size_t> Group;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(const Group&, const Group&) const;
	bool IsWeakChain(std::size_t, std::size_t) const;
	bool IsWeakChain(std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t) const;
	bool IsWeakChain(const Group&, const Group&) const;
	size_t ChainType(const std::vector<Group>&) const;
	void PrintChain(std::ostream&, const std::vector<Group>&);
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
	std::string GroupedForcingChain();
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

inline int Candidate::Difficulty() const {
	return difficulty;
}
