#pragma once
#include <string>
#include <vector>


namespace BQSudoku {
	class Sudoku;
}

std::ostream& operator<<(std::ostream&, const BQSudoku::Sudoku&);

namespace BQSudoku {
	class Sudoku {
	protected:
		std::size_t m, n;
		std::size_t size;
		std::vector<size_t> board;
		std::vector<bool> given;
	public:
		Sudoku(const Sudoku&);
		Sudoku(Sudoku&&);
		Sudoku& operator=(const Sudoku&);
		Sudoku& operator=(Sudoku&&);
		~Sudoku();
	public:
		Sudoku();
		Sudoku(std::size_t);
		Sudoku(std::size_t, std::size_t);
		Sudoku(std::size_t, const std::string&);
		Sudoku(std::size_t, std::size_t, const std::string&);
	public:
		bool Solved() const;
	protected:
		char Row2Char(std::size_t) const;
		char Column2Char(std::size_t) const;
		char Number2Char(std::size_t) const;
	public:
		inline std::size_t& operator()(std::size_t, std::size_t);
		inline std::size_t operator()(std::size_t, std::size_t) const;
		friend std::ostream& ::operator<<(std::ostream&, const Sudoku&);
	};

	inline std::size_t& Sudoku::operator()(std::size_t row, std::size_t column) {
		return board[row * size + column];
	}

	inline std::size_t
	Sudoku::operator()(std::size_t row, std::size_t column) const {
		return board[row * size + column];
	}
}
