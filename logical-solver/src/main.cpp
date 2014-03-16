#include <cstring>
#include <fstream>
#include <iostream>
#include "BruteForceSolver.h"
#include "Candidate.h"
#include "Sudoku.h"
using namespace std;


void CheckUniqueness(const Sudoku&);
void BruteForce(const Sudoku&);
void StepByStep(const Sudoku&, bool = false, bool = true);

int main(int argc, char **argv) {
	if (argc == 1) {
		cout << "BQSudoku" << endl;
		cout << "bqsudoku [option] [file]" << endl;
		cout << "Option: " << endl;
		cout << "\t--print / -p\t\t print sudoku" << endl;
		cout << "\t--check / -c\t\t check validity" << endl;
		cout << "\t--brute / -b\t\tbrute force solver" << endl;
		cout << "\t--step / -s\t\tstep by step solver" << endl;
		cout << "\t--step-multi / -m\t step by step solver when you can't determine it's uniqueness" << endl;
		cout << "\t-c\t\t\tprint candidates" << endl;
	} else {
		size_t l, m, n;
		string str;
		bool print_step = false;
		if (argc == 2) {
			cin >> l >> m >> n >> str;
		} else if (argc == 3) {
			if (strcmp(argv[2], "-c") == 0) {
				print_step = true;
				cin >> l >> m >> n >> str;
			} else {
				ifstream fin(argv[2]);
				fin >> l >> m >> n >> str;
			}
		} else if (argc == 4) {
			print_step = true;
			ifstream fin(argv[3]);
			fin >> l >> m >> n >> str;
		}
		if (l != m * n) {
			cerr << "Sudoku size does not match" << endl;
		} else if (str.length() != l * l) {
			cerr << "String length does not match" << endl;
		} else {
			Sudoku sudoku(m, n, str);
			cout << sudoku << endl;
			if (strcmp(argv[1], "--check") == 0 || strcmp(argv[1], "-c") == 0) {
				CheckUniqueness(sudoku);
			} else if (strcmp(argv[1], "--brute") == 0 || strcmp(argv[1], "-b") == 0) {
				BruteForce(sudoku);
			} else if (strcmp(argv[1], "--step") == 0 || strcmp(argv[1], "-s") == 0) {
				StepByStep(sudoku, print_step);
			} else if (strcmp(argv[1], "--step-multi") == 0 || strcmp(argv[1], "-m") == 0) {
				StepByStep(sudoku, print_step, false);
			}
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

void StepByStep(const Sudoku &sudoku, bool print_step, bool uniqueness) {
	Candidate sbssudoku(sudoku, uniqueness);
	bool newline = false;
	if (print_step) {
		cout << endl << sbssudoku << endl;
	}
	while (!sbssudoku.Solved()) {
		auto hint = sbssudoku.GetHint();
		if (!hint.first.empty()) {
			if (print_step) {
				cout << endl << hint.first << endl << sbssudoku << endl;
			} else {
				if (!newline) {
					cout << endl;
				}
				if (hint.second) {
					cout << hint.first << endl << static_cast<Sudoku>(sbssudoku) << endl;
					newline = false;
				} else {
					newline = true;
					cout << hint.first << endl;
				}
			}
		} else {
			break;
		}
	}
	if (sbssudoku.Solved()) {
		cout << endl << "Difficulty = " << sbssudoku.Difficulty() << endl;
	}
}
