#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "BruteForceSolver.h"
#include "Candidate.h"
#include "Sudoku.h"
using namespace std;
using namespace BQSudoku;


void CheckUniqueness(const Sudoku&);
void BruteForce(const Sudoku&);
void StepByStep(const Sudoku&);

int main(int argc, char **argv) {
	if (argc == 1) {
		cout << "BQSudoku" << endl;
		cout << "bqsudoku [option] [file]" << endl;
		cout << "Option: " << endl;
		cout << "\t--print / -p\t print sudoku" << endl;
		cout << "\t--check / -c\t check validity" << endl;
		cout << "\t--brute-force / -b\tbrute force solver" << endl;
		cout << "\t--step-by-step / -s\tstep by step solver" << endl;
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
		cout << sudoku << endl;
		if (strcmp(argv[1], "--check") == 0 || strcmp(argv[1], "-c") == 0) {
			CheckUniqueness(sudoku);
		} else if (strcmp(argv[1], "--brute-force") == 0 || strcmp(argv[1], "-b") == 0) {
			BruteForce(sudoku);
		} else if (strcmp(argv[1], "--step-by-step") == 0 || strcmp(argv[1], "-s") == 0) {
			StepByStep(sudoku);
		}
	}
	return 0;
}

void CheckUniqueness(const Sudoku &sudoku) {
	BruteForceSolver bfsudoku(sudoku, true);
	bfsudoku();
	switch (bfsudoku.answer_count) {
	case 0:
		cout << "No answer" << endl;
		break;
	case 1:
		cout << "There is an answer, and the answer is unique" << endl;
		break;
	default:
		cout << "There are multiple answers" << endl;
	}
}

void BruteForce(const Sudoku &sudoku) {
	BruteForceSolver bfsudoku(sudoku);
	bfsudoku();
	cout << endl << "Totally " << bfsudoku.answer_count << " answer(s)" << endl;
	for (const Sudoku &x: bfsudoku.answer) {
		cout << endl << x << endl;
	}
}

void StepByStep(const Sudoku &sudoku) {
	Candidate sbssudoku(sudoku);
	while (!sbssudoku.Solved()) {
		string str = sbssudoku.FindNext();
		if (!str.empty()) {
			cout << endl << str << endl << sbssudoku << endl;
		} else {
			break;
		}
	}
	if (sbssudoku.Solved()) {
		cout << endl << "Difficulty = " << sbssudoku.Difficulty() << endl;
	}
}
