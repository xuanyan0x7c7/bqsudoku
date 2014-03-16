function generalCompare(x, y) {
	if (x == y) {
		return 0;
	} else if (x < y) {
		return -1;
	} else {
		return 1;
	}
}

function sortNumber(x, y) {
	return x - y;
}

function Sudoku(variable, grid, candidate) {
	this.variable = [];
	if (variable instanceof Array) {
		this.variables = variable.length;
		var is_number = true;
		for (var i = 0; i < this.variables; ++i) {
			if (!(/^-?\d*$/.test(variable[i]))) {
				is_number = false;
				break;
			}
		}
		if (is_number) {
			for (var i = 0; i < this.variables; ++i) {
				this.variable.push(Number(variable[i]));
			}
			for (var i = 0; i < this.variables - 1; ++i) {
				for (var j = 1; j < this.variables; ++j) {
					if (this.variable[i] == this.variable[j]) {
						is_number = false;
						break;
					}
				}
			}
		}
		if (!is_number) {
			this.variable = variable.concat();
		}
	} else {
		this.variables = variable;
		for (var i = 0; i < this.variables; ++i) {
			this.variable.push(i);
		}
	}

	if (grid instanceof Array) {
		this.grids = grid.length;
		this.grid = grid.concat();
	} else {
		this.grids = grid;
		this.grid = [];
		for (var i = 0; i < grid; ++i) {
			this.grid.push(-1);
		}
	}

	this.candidate = [];
	for (var i = 0; i < this.grids * this.variables; ++i) {
		this.candidate.push(true);
	}
	if (arguments.length > 2) {
		for (var c = 0; c < candidate.length; ++c) {
			this.candidate[candidate[c]] = false;
		}
	}

	this.regions = 0;
	this.region = [];
	this.constraints = 0;
	this.constraint = [];
}

Sudoku.prototype.removeCandidate = function(cell, number) {
	this.candidate[cell * this.variables + number] = false;
};

Sudoku.prototype.addRegion = function(region, sorted) {
	if (arguments.length == 2 && sorted) {
		this.region[this.regions++] = region.concat();
	} else {
		this.region[this.regions++] = region.concat().sort(sortNumber);
	}
};

Sudoku.prototype.addRegionSatisfies = function(unary_prediction, begin, end) {
	if (arguments.length == 1) {
		begin = 0;
		end = this.grids;
	}
	var region = [];
	for (var i = begin; i < end; ++i) {
		if (unary_prediction(i)) {
			region.push(i);
		}
	}
	if (region.length > 1) {
		this.addRegion(region, true);
	}
};

Sudoku.prototype.addConstraint = function(constraint, sorted) {
	if (arguments.length == 2 && sorted) {
		this.constraint[this.constraints++] = constraint.concat();
	} else {
		this.constraint[this.constraints++] = constraint.concat().sort(sortNumber);
	}
};

Sudoku.prototype.addConstraintSatisfies = function(unary_prediction, begin, end) {
	if (arguments.length == 1) {
		begin = 0;
		end = this.grids * this.variables;
	}
	var constraint = [];
	for (var i = begin; i < end; ++i) {
		if (unary_prediction(i)) {
			constraint.push(i);
		}
	}
	if (constraint.length > 1) {
		this.addConstraint(constraint, true);
	}
};

Sudoku.prototype.addConstraintIf = function(binary_prediction, cell1, cell2) {
	var n = this.variables;
	for (var i = 0; i < n; ++i) {
		var constraint = [cell1 * n + i];
		for (var j = 0; j < n; ++j) {
			if (!binary_prediction(i, j)) {
				constraint.push(cell2 * n + j);
			}
		}
		if (constraint.length == 1) {
			this.removeCandidate(cell1, i);
		} else {
			this.addConstraint(constraint);
		}
	}
};

Sudoku.prototype.addComparisonConstraint = function(cell1, cell2, comparison) {
	var sudoku = this;
	this.addConstraintIf(function(x, y) {
		return generalCompare(sudoku.variable[x], sudoku.variable[y]) == generalCompare(comparison, 0);
	}, cell1, cell2);
};