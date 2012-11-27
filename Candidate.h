#pragma once
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include "Sudoku.h"


namespace BQSudoku {
	class Candidate;
}

std::ostringstream&
PrintChain(std::ostringstream&, const BQSudoku::Candidate&, std::list<size_t>);

namespace BQSudoku {
	class Candidate: public Sudoku {
	private:
		std::vector<std::vector<bool>> candidate;
		std::vector<size_t> row_count, column_count, box_count, cell_count;
		std::vector<std::vector<size_t>> row_index, column_index, box_index;
	public:
		Candidate(const Candidate&);
		Candidate(Candidate&&);
		Candidate& operator=(const Candidate&);
		Candidate& operator=(Candidate&&);
		~Candidate();
	public:
		Candidate(const Sudoku&);
	private:
		inline std::vector<bool>::reference operator()(std::size_t);
		inline std::vector<bool>::reference
		operator()(std::size_t, std::size_t);
		inline std::vector<bool>::reference
		operator()(std::size_t, std::size_t, std::size_t);
		friend std::ostringstream&
		::PrintChain(std::ostringstream&, const Candidate&, std::list<size_t>);
	public:
		inline bool operator()(std::size_t) const;
		inline bool operator()(std::size_t, std::size_t) const;
		inline bool operator()(std::size_t, std::size_t, std::size_t) const;
		void Fill(std::size_t, std::size_t);
		void Fill(std::size_t, std::size_t, std::size_t);
		void Remove(std::size_t);
		void Remove(std::size_t, std::size_t);
		void Remove(std::size_t, std::size_t, std::size_t);
		std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t);
		std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t,
			std::size_t, std::size_t);
		std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t,
			std::size_t, std::size_t, std::size_t, std::size_t);
		bool IsWeakChain(std::size_t, std::size_t);
		bool IsWeakChain(std::size_t, std::size_t, std::size_t, std::size_t,
			std::size_t, std::size_t);
		std::string FindNext();
	public:
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

	inline std::vector<bool>::reference
	Candidate::operator()(std::size_t number) {
		return candidate[number / size][number % size];
	}

	inline std::vector<bool>::reference
	Candidate::operator()(std::size_t index, std::size_t number) {
		return candidate[index][number - 1];
	}

	inline std::vector<bool>::reference Candidate::operator()(std::size_t row,
	std::size_t column, std::size_t number) {
		return candidate[row * size + column][number - 1];
	}

	inline bool Candidate::operator()(std::size_t number) const {
		return candidate[number / size][number % size];
	}

	inline bool
	Candidate::operator()(std::size_t index, std::size_t number) const {
		return candidate[index][number - 1];
	}

	inline bool Candidate::operator()(std::size_t row, std::size_t column,
		std::size_t number) const {
		return candidate[row * size + column][number - 1];
	}
}
