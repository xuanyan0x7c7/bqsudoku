var rows;
var columns;
var cell_size;
var cell_use;
var $cell_obj;
var region = [];
var constraint = [];
var grid = [];
var candidate = [];
var variable = [];
var step;

$(document).ready(function() {
	$(".nav-progress:not(#nav-set-board)").addClass("todo");
	$("#nav-set-board").addClass("current");
	$(".dropdown-toggle").dropdown();
	$("input#cell-size").attr({
		min: 10, max: 50, value: 35
	}).change(function() {
		changeView();
	});
	$("#table-div").bind({
		contextmenu: function() {return false;},
		dragstart: function() {return false;},
		selectstart: function() {return false;}
	});

	$("button#progress").click(function() {
		if (step == "set-board") {
			endBoard();
			$(".content#set-board").hide("normal");
			step = "add-regions";
			$(".content#add-regions").show("normal");
			beginRegion();
		} else if (step == "add-regions") {
			endRegion();
			$(".content#add-regions").hide("normal");
			step = "add-constraints";
			$(".content#add-constraints").show("normal");
			beginConstraint();
		} else if (step == "add-constraints") {
			endConstraint();
			$(".content#add-constraints").hide("normal");
			step = "set-givens";
			$(".content#set-givens").show("normal");
			beginGivens();
		} else if (step == "set-givens") {
			if (endGivens()) {
				$(".content#set-givens").hide("normal");
				step = "remove-candidates";
				$(".content#remove-candidates").show("normal");
				beginCandidates();
			}
		} else if (step == "remove-candidates") {
			$(".content#remove-candidates").hide("normal");
			step = "finish-creation";
			$(".content#finish-creation").show("normal");
			finish();
		}
	});
	$("button#solve").click(function() {solve();});
	$("button#abort").prop("disabled", "true").click(function() {abort();});
	$(".btn-group#solve-group").hide();
	$("button#prev-answer").prop("disabled", true).click(function() {previousAnswer();});
	$("button#next-answer").click(function() {nextAnswer();});
	$(".btn-group#answer-group").hide();

	$("#set-board input").attr({
		min: 2, value: 9
	}).change(function() {beginBoard();});
	$("#set-board button:first").click(function() {enableBoard(true);});
	$("#set-board button:last").click(function() {enableBoard(false);});
	$("#add-region-group > button:first").click(function() {regionAdd();});
	$("#add-region-group li:eq(0)").click(function() {regionAddRow();});
	$("#add-region-group li:eq(1)").click(function() {regionAddColumn();});
	$("#add-region-group li:eq(2)").click(function() {regionAddAntiKnights();});
	$("#add-regions button:last").click(function() {regionRemoveAll();});
	$("#add-constraint-group > button:eq(0)").click(function() {constraintAddComparison(0);});
	$("#add-constraint-group > button:eq(1)").click(function() {constraintAddComparison(-1);});
	$("#add-constraint-group > button:eq(2)").click(function() {constraintAddComparison(1);});
	$("#add-constraints button:last").click(function() {constraintRemoveAll();});
	$("set-givens button").click(function() {
		$("input.grid-input").val("");
	});
	$("#answer-div").hide();
	$(".content:not(#set-board)").hide();
	step = "set-board";
	beginBoard();
});

var map = [];
var inv_map = [];
var region_select = [];

function beginBoard() {
	rows = parseInt($("#rows").val());
	columns = parseInt($("#columns").val());
	cell_use = [];
	for (var i = 0; i < rows; ++i) {
		cell_use.push([]);
		for (var j = 0; j < columns; ++j) {
			cell_use[i].push(true);
		}
	}
	changeBoard($("#table-div"));
	changeView();
}

function endBoard() {
	$("table#table.sudoku td").unbind("mousedown mouseup");
}

function changeBoard($table_div) {
	$("#table").remove();
	var $table = $("<table>");
	$table.addClass("sudoku").attr("id", "table");
	var $tbody = $("<tbody>");
	$cell_obj = [];
	for (var i = 0; i < rows; ++i) {
		var $row = $("<tr>");
		$cell_obj.push([]);
		for (var j = 0; j < columns; ++j) {
			var $cell = $("<td>").appendTo($row);
			$cell_obj[i].push($cell);
		}
		$row.appendTo($tbody);
	}
	$tbody.appendTo($table.appendTo($table_div));
	$("#table td").attr("data-removed", false).each(function(index) {
		var r = Math.floor(index / columns);
		var c = index % columns;
		$(this).bind({
			mousedown: function() {boardMouseDown(r, c);},
			mouseup: function() {boardMouseUp(r, c);}
		});
	});
}

function changeView() {
	cell_size = parseInt($("#cell-size").val());
	$("table.sudoku td").css({
		width: cell_size,
		height: cell_size,
		"font-size": Math.floor(cell_size * 0.8)
	});
	$("input.grid-input").css({
		width: Math.floor(cell_size * 0.9),
		height: Math.floor(cell_size * 0.9),
		"font-size": Math.floor(cell_size * 0.8)
	});
	$("#table-div").each(function() {
		var table = $("#table");
		$(this).width(table.outerWidth() + 20).height(table.outerHeight() + 20);
	});
}

function enableBoard(use) {
	for (var i = 0; i < rows; ++i) {
		for (var j = 0; j < columns; ++j) {
			cell_use[i][j] = use;
			$cell_obj[i][j].attr("data-removed", !use);
		}
	}
}

function setBoard(r, c) {
	cell_use[r][c] = !cell_use[r][c];
	$cell_obj[r][c].attr("data-removed", !cell_use[r][c]);
}

var board_mouse_start = [-1, -1];
var board_choosing = false;

function boardMouseDown(r, c) {
	board_choosing = true;
	board_mouse_start = [r, c];
}

function boardMouseUp(r, c) {
	if (board_choosing) {
		var r0 = board_mouse_start[0];
		var c0 = board_mouse_start[1];
		if (r0 > r) {
			var temp = r0;
			r0 = r;
			r = temp;
		}
		if (c0 > c) {
			var temp = c0;
			c0 = c;
			c = temp;
		}
		for (var i = r0; i <= r; ++i) {
			for (var j = c0; j <= c; ++j) {
				setBoard(i, j);
			}
		}
	}
}

function beginRegion() {
	for (var i = 0; i < rows; ++i) {
		for (var j = 0; j < columns; ++j) {
			if (cell_use[i][j]) {
				inv_map[i * columns + j] = map.length;
				map.push(i * columns + j);
			} else {
				inv_map[i * columns + j] = -1;
			}
			region_select.push(false);
		}
	}
	$("#table td").attr("data-selected", "false").each(function(index) {
		var r = Math.floor(index / columns);
		var c = index % columns;
		$(this).bind({
			mousedown: function() {regionMouseDown(r, c);},
			mouseup: function() {regionMouseUp(r, c);}
		});
	});
	$("#nav-set-board").removeClass("current").addClass("done");
	$("#nav-add-regions").removeClass("todo").addClass("current");
}

function endRegion() {
	$("#table td").removeAttr("data-selected").unbind("mousedown mouseup");
}

function regionSelect(r, c) {
	if ($cell_obj[r][c].attr("data-selected") == "false") {
		$cell_obj[r][c].attr("data-selected", "true");
		region_select[r * columns + c] = true;
	} else {
		$cell_obj[r][c].attr("data-selected", "false");
		region_select[r * columns + c] = false;
	}
}

var region_mouse_start = [-1, -1];
var region_choosing = false;

function regionMouseDown(r, c) {
	if ($("#add-regions button[data-toggle=\"button\"]").hasClass("active")) {
		var reg = [];
		for (var i = 0; r + i < rows; ++i) {
			for (var j = 0; c + j < columns; ++j) {
				if ($cell_obj[i][j].attr("data-selected") == "true"
					&& cell_use[r + i][c + j]) {
					reg.push(inv_map[(r + i) * columns + (c + j)]);
				}
			}
		}
		if (reg.length > 1) {
			region.push(reg);
		}
		regionPrint($("p#regions"), true);
	} else {
		region_choosing = true;
		region_mouse_start = [r, c];
	}
}

function regionMouseUp(r, c) {
	if (region_choosing) {
		var r0 = region_mouse_start[0];
		var c0 = region_mouse_start[1];
		if (r0 > r) {
			var temp = r0;
			r0 = r;
			r = temp;
		}
		if (c0 > c) {
			var temp = c0;
			c0 = c;
			c = temp;
		}
		for (var i = r0; i <= r; ++i) {
			for (var j = c0; j <= c; ++j) {
				regionSelect(i, j);
			}
		}
		region_choosing = false;
	}
}

function regionAdd() {
	var reg = [];
	for (var i = 0; i < region_select.length; ++i) {
		var r = Math.floor(i / columns);
		var c = i % columns;
		if (region_select[i] && cell_use[r][c]) {
			reg.push(inv_map[i]);
		}
		region_select[i] = false;
		$cell_obj[r][c].attr("data-selected", "false");
	}
	if (reg.length > 1) {
		region.push(reg);
	}
	regionPrint($("p#regions"), true);
}

function regionPrint($div, mutable) {
	$div.html("");
	for (var i = 0; i < region.length; ++i) {
		if (mutable) {
			$div.append("<i>");
		}
		var str_arr = [];
		for (var j = 0; j < region[i].length; ++j) {
			var r = Math.floor(map[region[i][j]] / columns);
			var c = map[region[i][j]] % columns;
			str_arr.push(" (");
			str_arr.push(r + 1);
			str_arr.push(',');
			str_arr.push(c + 1);
			str_arr.push(')');
		}
		$("<span>").append(str_arr.join("")).appendTo($div);
		$div.append("<br>");
	}
	$div.children("i").addClass("icon-remove").each(function(index) {
		$(this).click(function() {regionRemove(index);});
	});
	$div.children("span").each(function(index) {
		$(this).bind({
			mouseenter: function() {regionMouseEnter(index);},
			mouseleave: function() {regionMouseLeave(index);}
		});
	});
}

function regionMouseEnter(x) {
	for (var i = 0; i < region[x].length; ++i) {
		var r = Math.floor(map[region[x][i]] / columns);
		var c = map[region[x][i]] % columns;
		$cell_obj[r][c].attr("data-hover", "true");
	}
}

function regionMouseLeave(x) {
	for (var i = 0; i < region[x].length; ++i) {
		var r = Math.floor(map[region[x][i]] / columns);
		var c = map[region[x][i]] % columns;
		$cell_obj[r][c].removeAttr("data-hover");
	}
}

function regionAddRow() {
	for (var i = 0; i < rows; ++i) {
		var reg = [];
		for (var j = 0; j < columns; ++j) {
			if (cell_use[i][j]) {
				reg.push(inv_map[i * columns + j]);
			}
		}
		if (reg.length > 1) {
			region.push(reg);
		}
	}
	regionPrint($("p#regions"), true);
}

function regionAddColumn() {
	for (var j = 0; j < columns; ++j) {
		var reg = [];
		for (var i = 0; i < rows; ++i) {
			if (cell_use[i][j]) {
				reg.push(inv_map[i * columns + j]);
			}
		}
		if (reg.length > 1) {
			region.push(reg);
		}
	}
	regionPrint($("p#regions"), true);
}

function regionAddAntiKnights() {
	var square = function (x) {
		return x * x;
	}
	for (var x = 0; x < rows * columns; ++x) {
		var r1 = Math.floor(x / columns);
		var c1 = x % columns;
		if (cell_use[r1][c1]) {
			for (var y = x + 1; y < rows * columns; ++y) {
				var r2 = Math.floor(y / columns);
				var c2 = y % columns;
				if (cell_use[r2][c2] && square(r1 - r2) + square(c1 - c2) == 5) {
					region.push([inv_map[x], inv_map[y]]);
				}
			}
		}
	}
	regionPrint($("p#regions"), true);
}

function regionRemove(index) {
	region.splice(index, 1);
	regionPrint($("p#regions"), true);
}

function regionRemoveAll() {
	region = [];
	regionPrint($("p#regions"), true);
}

function beginConstraint() {
	$("#table td").each(function(index) {
		var r = Math.floor(index / columns);
		var c = index % columns;
		if (cell_use[r][c]) {
			$(this).click(function() {constraintSelect(r, c);});
		}
	});
	$("#nav-add-regions").removeClass("current").addClass("done");
	$("#nav-add-constraints").removeClass("todo").addClass("current");
}

function endConstraint() {
	for (var i = 0; i < rows; ++i) {
		for (var j = 0; j < columns; ++j) {
			if (cell_use[i][j]) {
				$cell_obj[i][j].html("").unbind("click");
			}
		}
	}
}

function constraintPrint($div, mutable) {
	$div.html("");
	for (var i = 0; i < constraint.length; ++i) {
		if (mutable) {
			$div.append("<i>");
		}
		var con = constraint[i];
		var str_arr = [];
		str_arr.push(" (");
		str_arr.push(Math.floor(con.data[0] / columns) + 1);
		str_arr.push(',');
		str_arr.push(con.data[0] % columns + 1);
		str_arr.push(')');
		var op = " ";
		if (con.type == "equals to") {
			op = " = ";
		} else if (con.type == "less than") {
			op = " &lt; "
		} else if (con.type == "greater than") {
			op = " &gt; ";
		}
		for (var j = 1; j < con.data.length; ++j) {
			str_arr.push(op);
			str_arr.push('(');
			str_arr.push(Math.floor(con.data[j] / columns) + 1);
			str_arr.push(',');
			str_arr.push(con.data[j] % columns + 1);
			str_arr.push(')');
		}
		$div.append(str_arr.join("")).append("<br>");
	}
	$div.children("i").addClass("icon-remove").each(function(index) {
		$(this).click(function() {constraintRemove(index);});
	});
}

var constraint_select = [];

function constraintAddComparison(comparison) {
	if (constraint_select.length < 2) {
		return;
	}
	if (comparison == 0) {
		constraint.push({
			type: "equals to",
			data: constraint_select.concat()
		});
	} else if (comparison < 0) {
		constraint.push({
			type: "less than",
			data: constraint_select.concat()
		});
	} else {
		constraint.push({
			type: "greater than",
			data: constraint_select.concat()
		});
	}
	constraint_select = [];
	constraintPrint($("p#constraints"), true);
	constraintClearMarks();
}

function constraintSelect(r, c) {
	var found = false;
	for (var i = 0; i < constraint_select.length; ++i) {
		if (constraint_select[i] == r * columns + c) {
			constraint_select.splice(i, 1);
			found = true;
			break;
		}
	}
	if (!found) {
		constraint_select.push(r * columns + c);
	}
	constraintClearMarks();
	for (var i = 0; i < constraint_select.length; ++i) {
		var r0 = Math.floor(constraint_select[i] / columns);
		var c0 = constraint_select[i] % columns;
		$cell_obj[r0][c0].html("" + (i + 1));
	}
}

function constraintClearMarks() {
	for (var i = 0; i < rows; ++i) {
		for (var j = 0; j < columns; ++j) {
			if (cell_use[i][j]) {
				$cell_obj[i][j].html("");
			}
		}
	}
}

function constraintRemove(index) {
	constraint.splice(index, 1);
	constraintPrint($("p#constraints"), true);
}

function constraintRemoveAll() {
	constraint = [];
	constraintPrint($("p#constraints"), true);
}

function beginGivens() {
	for (var i = 0; i < rows; ++i) {
		for (var j = 0; j < columns; ++j) {
			if (cell_use[i][j]) {
				$cell_obj[i][j].append("<input>");
			}
		}
	}
	$("#table td > input").addClass("grid-input").css({
		"width": Math.floor(cell_size * 0.9),
		"height": Math.floor(cell_size * 0.9),
		"font-size": Math.floor(cell_size * 0.8)
	});
	$("#nav-add-constraints").removeClass("current").addClass("done");
	$("#nav-set-givens").removeClass("todo").addClass("current");
}

function endGivens() {
	var str = $("#variable").val();
	if (str == "") {
		return false;
	}
	var all_variable = [];
	var index = -1;
	do {
		var last_index = index;
		index = str.indexOf(' ', index + 1);
		var v = str.substring(last_index + 1, index >= 0 ? index : str.length);
		if (v != "") {
			all_variable.push(v);
		}
	} while (index != -1);
	if (all_variable.length == 0) {
		return false;
	}

	var variable_unique = [all_variable[0]];
	for (var i = 1; i < all_variable.length; ++i) {
		var ok = true;
		for (var j = 0; j < i; ++j) {
			if (all_variable[j] == all_variable[i]) {
				ok = false;
				break;
			}
		}
		if (ok) {
			variable_unique.push(all_variable[i]);
		}
	}
	for (var i = 0; i < region.length; ++i) {
		if (region[i].length > variable_unique.length) {
			return false;
		}
	}
	variable = variable_unique;

	$("input.grid-input").each(function() {
		$(this).prop("disabled", true);
		var str = $(this).val();
		if (str != "") {
			var found = false;
			for (var j = 0; j < variable.length; ++j) {
				if (str == variable[j]) {
					grid.push(j);
					found = true;
					break;
				}
			}
			if (!found) {
				grid.push(-1);
			}
		} else {
			grid.push(-1);
		}
	});
	for (var i = 0; i < map.length; ++i) {
		var r = Math.floor(map[i] / columns);
		var c = map[i] % columns;
		if (grid[i] >= 0) {
			$cell_obj[r][c].text(variable[grid[i]]).addClass("given");
		} else {
			$cell_obj[r][c].text("");
		}
	}
	return true;
}

var candidate = [];
var candidate_choose = [];

function beginCandidates() {
	$("#nav-set-givens").removeClass("current").addClass("done");
	$("#nav-remove-candidates").removeClass("todo").addClass("current");
	$("button#progress").html("<i class=\"icon-ok icon-white\"></i> Finish")
		.removeClass("btn-primary").addClass("btn-success");
	var $table = $("#variable-table");
	var $tbody = $("<tbody>").appendTo($table);
	var $row;
	for (var i = 0; i < variable.length; ++i) {
		if (i % 10 == 0) {
			$row = $("<tr>").appendTo($tbody);
		}
		$cell = $("<td>").appendTo($row);
		$cell.text(variable[i]).css("background-color", "lightblue");
	}
	$("#table").mouseleave(function() {candidateMouseLeave();});
	$("#table td").each(function(index) {
		candidate.push([]);
		candidate_choose.push(false);
		for (var i = 0; i < variable.length; ++i) {
			candidate[index].push(false);
		}
		$(this).bind({
			mouseenter: function() {candidateMouseEnter(index);},
			mouseleave: function() {candidateMouseLeave();},
		}).filter("[data-removed=\"false\"]").not(".given").click(function() {
			candidateChoose(index);
		});
	});
	$table.parent().bind({
		mouseenter: function() {candidateMouseEnter();},
		mouseleave: function() {candidateMouseLeave();}
	});
	$table.find("td").each(function(index) {
		$(this).click(function() {candidateSelect(index);});
	});
}

function candidateMouseEnter(index) {
	var $table = $("#variable-table td");
	if (arguments.length == 1) {
		var $cell = $cell_obj[Math.floor(index / columns)][index % columns];
		if ($cell.attr("data-removed") == "true") {
			$table.css("background-color", "red");
		} else if ($cell.hasClass("given")) {
			var c = $cell.text();
			var x = -1;
			while (variable[++x] != c);
			$table.css("background-color", "red").eq(x).css("background-color", "lightblue");
		} else {
			for (var i = 0; i < variable.length; ++i) {
				$table.eq(i).css("background-color", candidate[index][i] ? "red" : "lightblue");
			}
		}
	} else {
		var choose = [];
		for (var i = 0; i < candidate_choose.length; ++i) {
			if (candidate_choose[i]) {
				choose.push(i);
			}
		}
		if (choose.length == 0) {
			return;
		}
		for (var i = 0; i < variable.length; ++i) {
			$table.eq(i).css("background-color", "red");
			for (var j = 0; j < choose.length; ++j) {
				if (!candidate[choose[j]][i]) {
					for (var k = 0; k < choose.length; ++k) {
						candidate[choose[k]][i] = false;
					}
					$table.eq(i).css("background-color", "lightblue");
					break;
				}
			}
		}
	}
}

function candidateMouseLeave() {
	$("#variable-table td").css("background-color", "lightblue");
}

function candidateChoose(index) {
	if (candidate_choose[index]) {
		candidate_choose[index] = false;
		$cell_obj[Math.floor(index / columns)][index % columns].css("background-color", "white");
	} else {
		candidate_choose[index] = true;
		$cell_obj[Math.floor(index / columns)][index % columns].css("background-color", "lightgreen");
	}
}

function candidateSelect(index) {
	var $table = $("#variable-table td");
	var choose = [];
	for (var i = 0; i < candidate_choose.length; ++i) {
		if (candidate_choose[i]) {
			choose.push(i);
		}
	}
	if (choose.length == 0) {
		return;
	}
	for (var i = 0; i < variable.length; ++i) {
		$table.eq(i).css("background-color", "red");
		for (var j = 0; j < choose.length; ++j) {
			if (!candidate[choose[j]][i]) {
				for (var k = 0; k < choose.length; ++k) {
					candidate[choose[k]][i] = false;
				}
				$table.eq(i).css("background-color", "lightblue");
				break;
			}
		}
	}
	if (candidate[choose[0]][index]) {
		for (var i = 0; i < choose.length; ++i) {
			candidate[choose[i]][index] = false;
			$table.eq(index).css("background-color", "lightblue");
		}
	} else {
		for (var i = 0; i < choose.length; ++i) {
			candidate[choose[i]][index] = true;
			$table.eq(index).css("background-color", "red");
		}
	}
}

function finish() {
	$("#nav-remove-candidates").removeClass("current").addClass("done");
	$("#nav-finish").removeClass("todo").addClass("current");
	$("button#progress").hide("normal");
	$(".btn-group#solve-group").show("normal");
}

var worker = null;
var answer = [];
var answers = 0;

function solve() {
	if (worker === null) {
		$("button#solve").prop("disabled", true);
		$("button#abort").prop("disabled", false);
		worker = "waiting";
		var sudoku = new Sudoku(variable.length, grid);
		for (var i = 0; i < rows * columns; ++i) {
			for (var j = 0; j < variable.length; ++j) {
				if (candidate[i][j]) {
					sudoku.removeCandidate(inv_map[i], j);
				}
			}
		}
		for (var i = 0; i < region.length; ++i) {
			sudoku.addRegion(region[i], true);
		}
		for (var i = 0; i < constraint.length; ++i) {
			var c = constraint[i];
			if (c.type == "equals to") {
				for (var j = 0; j < c.data.length - 1; ++j) {
					sudoku.addComparisonConstraint(c.data[j], c.data[j + 1], 0);
				}
			} else if (c.type == "less than") {
				for (var j = 0; j < c.data.length - 1; ++j) {
					sudoku.addComparisonConstraint(c.data[j], c.data[j + 1], -1);
				}
			} else if (c.type == "greater than") {
				for (var j = 0; j < c.data.length - 1; ++j) {
					sudoku.addComparisonConstraint(c.data[j], c.data[j + 1], 1);
				}
			}
		}
		console.time("time");
		worker = new Worker("bfsolver.js");
		worker.postMessage({sudoku: sudoku, unique: 2});
		worker.onmessage = function(e) {
			console.timeEnd("time");
			answer = e.data.answer;
			answers = e.data.answers;
			solveEnd();
			worker = null;
			$("button#solve").prop("disabled", false);
			$("button#abort").prop("disabled", true);
		}
	}
}

function abort() {
	worker.terminate();
	worker = null;
	$("button#solve").prop("disabled", false);
	$("button#abort").prop("disabled", true);
}

function solveEnd() {
	$(".btn-group#solve-group").hide("normal");
	$(".btn-group#answer-group").show("normal");
	$("button#next-answer").prop("disabled", answers < 2);
	if (answers == 1) {
		$("p#answer-info").html("<i class=\"icon-ok\"></i> \
			The sudoku has an answer, and the answer is unique!");
	} else {
		$("p#answer-info").html("The sudoku has " + answers + " answers.");
	}
	regionPrint($("#region-info"), false);
	constraintPrint($("#constraint-info"), false);
	$("#answer-div").show("normal");
	printAnswer(0);
}

var answer_index = 0;

function printAnswer(index) {
	for (var i = 0; i < grid.length; ++i) {
		var r = Math.floor(map[i] / columns);
		var c = map[i] % columns;
		$cell_obj[r][c].text(variable[answer[index][i]]);
	}
}

function previousAnswer() {
	if (answer_index > 0) {
		printAnswer(--answer_index);
		$("button#prev-answer").prop("disabled", answer_index == 0);
		$("button#next-answer").prop("disabled", false);
	}
}

function nextAnswer() {
	if (answer_index < answers - 1) {
		printAnswer(++answer_index);
		$("button#prev-answer").prop("disabled", false);
		$("button#next-answer").prop("disabled", answer_index == answers - 1);
	}
}
