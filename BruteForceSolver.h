#pragma once
#include <vector>
#include "Sudoku.h"


class BruteForceSolver: public Sudoku {
private:
	struct NodeType;
	typedef NodeType* Node;
	struct NodeType {
		std::size_t row, column;
		Node up, down, left, right;
	};
	const std::size_t rows, columns;
	Node head;
	std::vector<Node> node;
	std::vector<Node> column_head;
	std::vector<std::size_t> column_count;
	std::vector<bool> row_choose;
	const bool check_uniqueness;
public:
	std::vector<Sudoku> answer;
	unsigned long long answer_count;
public:
	BruteForceSolver(const BruteForceSolver&);
	BruteForceSolver(BruteForceSolver&&);
	BruteForceSolver& operator=(const BruteForceSolver&);
	BruteForceSolver& operator=(BruteForceSolver&&);
	~BruteForceSolver();
public:
	BruteForceSolver(const Sudoku&, bool check_uniqueness = false);
private:
	void Remove(size_t);
	void Resume(size_t);
	void Search(int);
	void AnswerFound();
public:
	void operator()();
};
