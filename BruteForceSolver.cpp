#include "BruteForceSolver.h"
using std::size_t;
using std::vector;


BruteForceSolver::BruteForceSolver(const BruteForceSolver&) = default;
BruteForceSolver::BruteForceSolver(BruteForceSolver&&) = default;
BruteForceSolver& BruteForceSolver::operator =(const BruteForceSolver&) = delete;
BruteForceSolver& BruteForceSolver::operator =(BruteForceSolver&&) = delete;

BruteForceSolver::~BruteForceSolver() {
	delete head;
	for (size_t i = 0; i < rows; ++i) {
		delete node[i];
	}
	for (size_t i = 0; i < columns; ++i) {
		delete column_head[i];
	}
}

BruteForceSolver::BruteForceSolver(const Sudoku &sudoku, bool check_uniqueness):
	Sudoku(sudoku), rows(size * size * size), columns(4 * size * size),
	node(4 * rows), column_head(columns), column_count(columns, size), row_choose(rows),
	check_uniqueness(check_uniqueness), answer_count(0) {
	head = new NodeType;
	head->left = head->right = head;

	for (size_t i = 0; i < columns; ++i) {
		column_head[i] = new NodeType;
		column_head[i]->column = i;
		column_head[i]->right = head;
		column_head[i]->left = head->left;
		column_head[i]->left->right = column_head[i];
		head->left = column_head[i];
		column_head[i]->up = column_head[i]->down = column_head[i];
	}

	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			for (size_t k = 0; k < size; ++k) {
				size_t column = ((i * size + j) * size + k) << 2;
				size_t index = column;
				node[index] = new NodeType;
				node[column]->left = node[column]->right = node[column];
				node[index]->row = column >> 2;
				node[index]->column = i * size + k;
				node[index]->left = node[column]->left;
				node[index]->right = node[column];
				node[index]->left->right = node[index];
				node[column]->left = node[index];
				node[index]->down = column_head[node[index]->column];
				node[index]->up = column_head[node[index]->column]->up;
				node[index]->up->down = node[index];
				column_head[node[index]->column]->up = node[index];
				node[++index] = new NodeType;
				node[index]->row = column >> 2;
				node[index]->column = size * size + j * size + k;
				node[index]->left = node[column]->left;
				node[index]->right = node[column];
				node[index]->left->right = node[index];
				node[column]->left = node[index];
				node[index]->down = column_head[node[index]->column];
				node[index]->up = column_head[node[index]->column]->up;
				node[index]->up->down = node[index];
				column_head[node[index]->column]->up = node[index];
				node[++index] = new NodeType;
				node[index]->row = column >> 2;
				node[index]->column = 2 * size * size + (i / m * m + j / n) * size + k;
				node[index]->left = node[column]->left;
				node[index]->right = node[column];
				node[index]->left->right = node[index];
				node[column]->left = node[index];
				node[index]->down = column_head[node[index]->column];
				node[index]->up = column_head[node[index]->column]->up;
				node[index]->up->down = node[index];
				column_head[node[index]->column]->up = node[index];
				node[++index] = new NodeType;
				node[index]->row = column >> 2;
				node[index]->column = 3 * size * size + i * size + j;
				node[index]->left = node[column]->left;
				node[index]->right = node[column];
				node[index]->left->right = node[index];
				node[column]->left = node[index];
				node[index]->down = column_head[node[index]->column];
				node[index]->up = column_head[node[index]->column]->up;
				node[index]->up->down = node[index];
				column_head[node[index]->column]->up = node[index];
			}
		}
	}
}

void BruteForceSolver::Remove(size_t column) {
	Node &column_head_node = column_head[column];
	column_head_node->left->right = column_head_node->right;
	column_head_node->right->left = column_head_node->left;
	for (Node row_node = column_head_node->down; row_node != column_head_node; row_node = row_node->down) {
		for (Node column_node = row_node->right; column_node != row_node; column_node = column_node->right) {
			--column_count[column_node->column];
			column_node->up->down = column_node->down;
			column_node->down->up = column_node->up;
		}
	}
}

void BruteForceSolver::Resume(size_t column) {
	Node &column_head_node = column_head[column];
	for (Node row_node = column_head_node->up; row_node != column_head_node; row_node = row_node->up) {
		for (Node column_node = row_node->left; column_node != row_node; column_node = column_node->left) {
			++column_count[column_node->column];
			column_node->up->down = column_node;
			column_node->down->up = column_node;
		}
	}
	column_head_node->left->right = column_head_node;
	column_head_node->right->left = column_head_node;
}

void BruteForceSolver::Search(int depth) {
	if (check_uniqueness && answer_count == 2) {
		return;
	} else if (head->right == head) {
		AnswerFound();
		return;
	}

	size_t min = size + 1;
	size_t column = 0;

	for (Node column_node = head->right; column_node != head; column_node = column_node->right) {
		if (column_count[column_node->column] < min) {
			column = column_node->column;
			min = column_count[column];
			if (min == 1) {
				break;
			} else if (min == 0) {
				return;
			}
		}
	}

	Remove(column);
	for (Node row_node = column_head[column]->down; row_node != column_head[column]; row_node = row_node->down) {
		row_choose[row_node->row] = true;
		for (Node column_node = row_node->right; column_node != row_node; column_node = column_node->right) {
			Remove(column_node->column);
		}
		Search(depth + 1);
		for (Node column_node = row_node->left; column_node != row_node; column_node = column_node->left) {
			Resume(column_node->column);
		}
		row_choose[row_node->row] = false;
	}
	Resume(column);
}

void BruteForceSolver::operator ()() {
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			size_t number = board[i * size + j];
			if (number > 0) {
				Remove(i * size + number - 1);
				Remove(size * size + j * size + number - 1);
				Remove(2 * size * size + (i / m * m + j / n) * size + number - 1);
				Remove(3 * size * size + i * size + j);
				row_choose[(i * size + j) * size + number - 1] = true;
			}
		}
	}
	Search(0);
}

void BruteForceSolver::AnswerFound() {
	if (!check_uniqueness) {
		for (size_t i = 0; i < rows; ++i) {
			if (row_choose[i]) {
				size_t row = i / (size * size);
				size_t column = (i / size) % size;
				size_t number = i % size;
				board[row * size + column] = number + 1;
			}
		}
		answer.push_back(*this);
	}
	++answer_count;
}
