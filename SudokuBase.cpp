#include "SudokuBase.h"
using std::size_t;


SudokuBase::SudokuBase(const SudokuBase&) = default;
SudokuBase::SudokuBase(SudokuBase&&) = default;
SudokuBase& SudokuBase::operator =(const SudokuBase&) = delete;
SudokuBase& SudokuBase::operator =(SudokuBase&&) = delete;
SudokuBase::~SudokuBase() = default;

SudokuBase::SudokuBase(): SudokuBase(3, 3) {}
SudokuBase::SudokuBase(size_t n): SudokuBase(n, n) {}
SudokuBase::SudokuBase(size_t m, size_t n): m(m), n(n), size(m * n) {}
