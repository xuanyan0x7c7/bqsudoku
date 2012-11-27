#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "BruteForceSolver.h"
#include "Candidate.h"
#include "Sudoku.h"
using namespace std;
using namespace BQSudoku;


void BruteForce(const Sudoku&);
void StepByStep(const Sudoku&);

int main(int argc, char **argv) {
	if (argc == 1) {
		cout << "--brute-force / --step-by-step" << endl;
	} else {
		size_t l, m, n;
		string str;
		if (argc == 2) {
			cin >> l >> m >> n >> str;
		} else if (argc == 3) {
			ifstream fin(argv[2]);
			fin >> l >> m >> n >> str;
		}
		Sudoku sudoku(m, n, str);
		cout << sudoku << endl << endl;
		if (strcmp(argv[1], "--brute-force") == 0
			|| strcmp(argv[1], "-b") == 0) {
			BruteForce(sudoku);
		} else if (strcmp(argv[1], "--step-by-step") == 0
			|| strcmp(argv[1], "-s") == 0) {
			StepByStep(sudoku);
		}
	}
	return 0;
}

void BruteForce(const Sudoku &sudoku) {
	BruteForceSolver bfsudoku(sudoku);
	bfsudoku();
	cout << "Totally " << bfsudoku.answer_count << " answer(s)" << endl;
	for (const Sudoku &x: bfsudoku.answer) {
		cout << endl << x << endl;
	}
}

void StepByStep(const Sudoku &sudoku) {
	Candidate sbssudoku(sudoku);
	while (!sbssudoku.Solved()) {
		string str = sbssudoku.FindNext();
		if (!str.empty()) {
			cout << str << endl << sbssudoku << endl;
		} else {
			break;
		}
	}
}
