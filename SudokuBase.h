#pragma once
#include <cstdlib>


class SudokuBase {
protected:
	const std::size_t m, n;
	const std::size_t size;
public:
	SudokuBase(const SudokuBase&);
	SudokuBase(SudokuBase&&);
	SudokuBase& operator =(const SudokuBase&);
	SudokuBase& operator =(SudokuBase&&);
	~SudokuBase();
public:
	SudokuBase();
	SudokuBase(size_t);
	SudokuBase(size_t, size_t);
protected:
	inline char Row2Char(std::size_t) const;
	inline char Column2Char(std::size_t) const;
	inline char Number2Char(std::size_t) const;
};

inline char SudokuBase::Row2Char(std::size_t row) const {
	return static_cast<char>('A' + row);
}

inline char SudokuBase::Column2Char(std::size_t column) const {
	return static_cast<char>((size <= 9 ? '1' : 'a') + column);
}

inline char SudokuBase::Number2Char(std::size_t number) const {
	if (number == 0) {
		return '.';
	} else if (size <= 9) {
		return static_cast<char>('0' + number);
	} else {
		return static_cast<char>('A' - 1 + number);
	}
}
