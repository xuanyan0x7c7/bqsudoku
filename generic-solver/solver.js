var sudoku = null;
var worker = null;
var output = [];

function initOutput() {
	output = [];
	for (var i = 0; i < 45; ++i) {
		output.push([]);
		for (var j = 0; j < 45; ++j) {
			output[i].push(-1);
		}
	}
	var offset = [[0, 18], [6, 24], [12, 30], [18, 36], [24, 30], [30, 24], [36, 18], [30, 12], [24, 6], [18, 0], [12, 6], [6, 12]];
	for (var k = 12; --k >= 0;) {
		for (var i = 0; i < 9; ++i) {
			for (var j = 0; j < 9; ++j) {
				output[i + offset[k][0]][j + offset[k][1]] = k * 81 + i * 9 + j;
			}
		}
	}
}

function print(object, grid) {
	var str = "";
	for (var i = 0; i < 45; ++i) {
		for (var j = 0; j < 45; ++j) {
			if (output[i][j] == -1) {
				str += "&nbsp;";
			} else {
				var x = grid[output[i][j]];
				if (x == -1) {
					str += "&middot;";
				} else {
					str += x + 1;
				}
			}
			if (j == 44) {
				str += "<br>";
			} else {
				str += "&nbsp;";
			}
		}
	}
	object.innerHTML = str;
}

var grid_str = [
"8.2.41.75.15....6..7...8.9.5.37..6.96.....45.74....2....6..7......4.........1....",
"....85.36....13.74..8624.1.48..5.3.7.1...8.....5.....8.....2....5..3....274......",
"...387......6...83....4.7..437.....5.9..146....6..3.9...9..6...16........749.8...",
"...91.7.6....8...........89.46..7.9..19348....2.6...7.....3.........9..4......832",
".51.9.....9...6.....2.47......46.........53..5..93..76...6...2....7...........7.1",
"83........6.481...91..3........24691.8.......2.156..8.............2.8..4.....6.1.",
"...43.......2...............581...4.6..9..7....9...8...21.9......582........7.5..",
"....7..1.......3......8..72..28169.5....5...1.91..4..69..1.....2.8.9.....6..28...",
"........2...2..46.....5.93.7..1........6...4..4..3....1.7.24.....4..1......79....",
"2..8......6.12....7..........298.631...7.25.....5....8937.......286.....6...9....",
"4.6.19.....8.3......34......5..4...2..4..16536.....1...............6.54......4.7.",
".....4..6.4.2.......1.73....5..879....2..1.......2651.....4..6....65..9....13.74."];

function init() {
	initOutput();
	var grid = [];
	for (var j = 0; j < 12; ++j) {
		for (var i = 0; i < 81; ++i) {
			var c = grid_str[j][i];
			grid.push(c == '.' ? -1 : "123456789".indexOf(c));
		}
	}
	sudoku = new Sudoku(9, grid);
	for (var k = 0; k < 12; ++k) {
		for (var i = 0; i < 9; ++i) {
			sudoku.addRegionSatisfies(function(x) {
				return Math.floor((x % 81) / 9) == i;
			}, 81 * k, 81 * (k + 1));
		}
		for (var i = 0; i < 9; ++i) {
			sudoku.addRegionSatisfies(function(x) {
				return Math.floor((x % 81) % 9) == i;
			}, 81 * k, 81 * (k + 1));
		}
		for (var i = 0; i < 3; ++i) {
			for (var j = 0; j < 3; ++j) {
				sudoku.addRegionSatisfies(function(x) {
					return Math.floor((x % 81) / 9 / 3) == i && Math.floor(((x % 81) % 9) / 3) == j;
				}, 81 * k, 81 * (k + 1));
			}
		}
	}
	for (var i = 0; i < 3; ++i) {
		for (var j = 0; j < 3; ++j) {
			sudoku.addComparisonConstraint((i + 6) * 9 + (j + 6), 81 + i * 9 + j, 0);
			sudoku.addComparisonConstraint(81 + (i + 6) * 9 + (j + 6), 2 * 81 + i * 9 + j, 0);
			sudoku.addComparisonConstraint(2 * 81 + (i + 6) * 9 + (j + 6), 3 * 81 + i * 9 + j, 0);
			sudoku.addComparisonConstraint(3 * 81 + (i + 6) * 9 + j, 4 * 81 + i * 9 + (j + 6), 0);
			sudoku.addComparisonConstraint(4 * 81 + (i + 6) * 9 + j, 5 * 81 + i * 9 + (j + 6), 0);
			sudoku.addComparisonConstraint(5 * 81 + (i + 6) * 9 + j, 6 * 81 + i * 9 + (j + 6), 0);
			sudoku.addComparisonConstraint(6 * 81 + i * 9 + j, 7 * 81 + (i + 6) * 9 + (j + 6), 0);
			sudoku.addComparisonConstraint(7 * 81 + i * 9 + j, 8 * 81 + (i + 6) * 9 + (j + 6), 0);
			sudoku.addComparisonConstraint(8 * 81 + i * 9 + j, 9 * 81 + (i + 6) * 9 + (j + 6), 0);
			sudoku.addComparisonConstraint(9 * 81 + i * 9 + (j + 6), 10 * 81 + (i + 6) * 9 + j, 0);
			sudoku.addComparisonConstraint(10 * 81 + i * 9 + (j + 6), 11 * 81 + (i + 6) * 9 + j, 0);
			sudoku.addComparisonConstraint(11 * 81 + i * 9 + (j + 6), (i + 6) * 9 + j, 0);
		}
	}
	print(document.getElementById("sudoku"), grid);
}

function solve() {
	if (worker === null) {
		document.getElementById("solve").disabled = true;
		document.getElementById("abort").disabled = false;
		console.time("time");
		worker = new Worker("bfsolver.js");
		worker.postMessage({sudoku: sudoku, unique: 2});
		worker.onmessage = function(e) {
			console.timeEnd("time");
			if (e.data.length == 1) {
				print(document.getElementById("answer"), e.data[0]);
			} else {
				document.getElementById("answer").innerHTML = "The sudoku has " + e.data.length + " solutions.";
			}
			document.getElementById("solve").disabled = false;
			document.getElementById("abort").disabled = true;
			worker = null;
		}
	}
}

function abort() {
	worker.terminate();
	worker = null;
	document.getElementById("solve").disabled = false;
	document.getElementById("abort").disabled = true;
}