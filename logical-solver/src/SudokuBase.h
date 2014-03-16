#pragma once
#include <string>


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
	SudokuBase(std::size_t);
	SudokuBase(std::size_t, std::size_t);
protected:
	inline std::string Row2String(std::size_t) const;
	inline std::string Column2String(std::size_t) const;
	inline std::string Cell2String(std::size_t) const;
	inline std::string Number2String(std::size_t) const;
};

inline std::string SudokuBase::Row2String(std::size_t row) const {
	return std::string(1, static_cast<char>('A' + row));
}

inline std::string SudokuBase::Column2String(std::size_t column) const {
	return std::string(1, static_cast<char>((size <= 9 ? '1' : 'a') + column));
}

inline std::string SudokuBase::Cell2String(std::size_t cell) const {
	return Row2String(cell / size) + Column2String(cell % size);
}

inline std::string SudokuBase::Number2String(std::size_t number) const {
	if (number == 0) {
		return ".";
	} else if (size <= 9) {
		return std::string(1, static_cast<char>('0' + number));
	} else {
		return std::string(1, static_cast<char>('A' - 1 + number));
	}
}
