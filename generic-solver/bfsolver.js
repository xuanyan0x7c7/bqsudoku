Array.prototype.binarySearch = function(value) {
	var left = 0;
	var right = this.length - 1;
	while (left <= right) {
		var mid = (left + right) >>> 1;
		if (value == this[mid]) {
			return true;
		} else if (value > this[mid]) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
	return false;
};

function Node(row, column, row_head, column_head) {
	this.row = row;
	this.column = column;
	if (row_head === null) {
		this.left = this.right = this;
	} else {
		this.left = row_head.left;
		this.right = row_head;
		this.left.right = this;
		row_head.left = this;
	}
	if (column_head === null) {
		this.up = this.down = this;
	} else {
		this.up = column_head.up;
		this.down = column_head;
		this.up.down = this;
		column_head.up = this;
	}
}

function Solver(sudoku, unique) {
	this.sudoku = sudoku;
	this.row_choose = [];
	for (var i = 0; i < sudoku.grids; ++i) {
		this.row_choose.push(-1);
	}
	this.node = [];
	this.head = new Node(-1, -1, null, null);
	this.column_head = [];
	this.column_count = [];
	var columns = -1;
	this.unique = unique;
	this.answer = [];
	this.answers = 0;

	for (var i = 0; i < sudoku.grids; ++i) {
		this.column_head.push(new Node(-1, ++columns, this.head, null));
		this.column_count.push(sudoku.variables);
	}
	for (var r = 0; r < sudoku.regions; ++r) {
		for (var i = 0; i < sudoku.variables; ++i) {
			if (sudoku.region[r].length == sudoku.variables) {
				this.column_head.push(new Node(-1, ++columns, this.head, null));
			} else {
				this.column_head.push(new Node(-1, ++columns, null, null));
			}
			this.column_count.push(sudoku.region[r].length);
		}
	}
	for (var c = 0; c < sudoku.constraints; ++c) {
		this.column_head.push(new Node(-1, ++columns, null, null));
		this.column_count.push(sudoku.constraint[c].length);
	}

	var column_index = -1;
	for (var i = 0; i < sudoku.grids; ++i) {
		var column_head = this.column_head[++column_index];
		for (var j = 0; j < sudoku.variables; ++j) {
			this.node.push(new Node(i * sudoku.variables + j, column_index, null, column_head));
		}
	}
	for (var r = 0; r < sudoku.regions; ++r) {
		for (var i = 0; i < sudoku.variables; ++i) {
			var column_head = this.column_head[++column_index];
			for (var cell = 0; cell < sudoku.region[r].length; ++cell) {
				var row = sudoku.region[r][cell] * sudoku.variables + i;
				this.node.push(new Node(row, column_index, this.node[row], column_head));
			}
		}
	}
	for (var c = 0; c < sudoku.constraints; ++c) {
		var column_head = this.column_head[++column_index];
		for (var cell = 0; cell < sudoku.constraint[c].length; ++cell) {
			var candidate = sudoku.constraint[c][cell];
			this.node.push(new Node(candidate, column_index, this.node[candidate], column_head));
		}
	}

	for (var cell = 0; cell < sudoku.grids; ++cell) {
		var number = sudoku.grid[cell];
		if (number >= 0) {
			this.remove(cell);
			for (var r = 0; r < this.sudoku.regions; ++r) {
				if (sudoku.region[r].binarySearch(cell)) {
					this.remove(sudoku.grids + r * sudoku.variables + number);
				}
			}
			for (var c = 0; c < this.sudoku.constraints; ++c) {
				if (sudoku.constraint[c].binarySearch(cell * sudoku.variables + number)) {
					this.remove(sudoku.grids + sudoku.regions * sudoku.variables + c);
				}
			}
			this.row_choose[cell] = number;
		}
	}

	for (var c = 0; c < sudoku.candidate.length; ++c) {
		if (!sudoku.candidate[c] && sudoku.grid[Math.floor(c / sudoku.variables)] == -1) {
			var row_head = this.node[c];
			if (row_head.up.down == row_head) {
				--this.column_count[row_head.column];
				row_head.up.down = row_head.down;
				row_head.down.up = row_head.up;
			}
			for (var node = row_head.right; node != row_head; node = node.right) {
				if (node.up.down == node) {
					--this.column_count[node.column];
					node.up.down = node.down;
					node.down.up = node.up;
				}
			}
		}
	}
}

Solver.prototype.remove = function(column) {
	var column_head = this.column_head[column];
	column_head.left.right = column_head.right;
	column_head.right.left = column_head.left;
	for (var row_node = column_head.down; row_node != column_head; row_node = row_node.down) {
		for (var column_node = row_node.right; column_node != row_node; column_node = column_node.right) {
			--this.column_count[column_node.column];
			column_node.up.down = column_node.down;
			column_node.down.up = column_node.up;
		}
	}
};

Solver.prototype.resume = function(column) {
	var column_head = this.column_head[column];
	for (var row_node = column_head.up; row_node != column_head; row_node = row_node.up) {
		for (var column_node = row_node.left; column_node != row_node; column_node = column_node.left) {
			++this.column_count[column_node.column];
			column_node.up.down = column_node;
			column_node.down.up = column_node;
		}
	}
	column_head.left.right = column_head;
	column_head.right.left = column_head;
};

Solver.prototype.search = function() {
	if (this.unique == 0 && this.answers == 2) {
		return;
	} else if (this.head.right == this.head) {
		this.answerFound();
		return;
	}

	var min = this.sudoku.variables + 1;
	var column = 0;

	for (var column_node = this.head.right; column_node != this.head; column_node = column_node.right) {
		if (this.column_count[column_node.column] < min) {
			column = column_node.column;
			min = this.column_count[column];
			if (min == 1) {
				break;
			} else if (min == 0) {
				return;
			}
		}
	}

	this.remove(column);
	for (var row_node = this.column_head[column].down; row_node != this.column_head[column];
			row_node = row_node.down) {
		this.row_choose[Math.floor(row_node.row / this.sudoku.variables)]
			= row_node.row % this.sudoku.variables;
		for (var column_node = row_node.right; column_node != row_node; column_node = column_node.right) {
			this.remove(column_node.column);
		}
		this.search();
		for (var column_node = row_node.left; column_node != row_node; column_node = column_node.left) {
			this.resume(column_node.column);
		}
		this.row_choose[Math.floor(row_node.row / this.sudoku.variables)] = -1;
	}
	this.resume(column);
}

Solver.prototype.solve = function() {
	this.search();
	return {answers: this.answers, answer: this.answer};
};

Solver.prototype.answerFound = function() {
	++this.answers;
	if (this.unique == 2 || this.answer.length < 2) {
		this.answer.push(this.row_choose.concat());
	}
};

self.onmessage = function(e) {
	postMessage(new Solver(e.data.sudoku, e.data.unique).solve());
};
