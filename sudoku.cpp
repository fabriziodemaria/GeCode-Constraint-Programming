#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include <string>
#include <cmath>
#include <cctype>

const int MSIZE = 9;
const int SQRSIZE = 3;

//Change this value to select the problem amongst the ones in "examples"
const int PROBLEM_NUM = 0;

using namespace Gecode;

//
// Help functions for ID2204, Assignment 1, Task 5
//

/* Sudoku specifications
*
* Each specification gives the initial positions that are filled in,
* with blank squares represented as zeroes.
*/
static int examples[][9][9] = {
		{
			{ 0, 0, 0, 2, 0, 5, 0, 0, 0 },
			{ 0, 9, 0, 0, 0, 0, 7, 3, 0 },
			{ 0, 0, 2, 0, 0, 9, 0, 6, 0 },

			{ 2, 0, 0, 0, 0, 0, 4, 0, 9 },
			{ 0, 0, 0, 0, 7, 0, 0, 0, 0 },
			{ 6, 0, 9, 0, 0, 0, 0, 0, 1 },

			{ 0, 8, 0, 4, 0, 0, 1, 0, 0 },
			{ 0, 6, 3, 0, 0, 0, 0, 8, 0 },
			{ 0, 0, 0, 6, 0, 8, 0, 0, 0 }
		}, {
				{ 3, 0, 0, 9, 0, 4, 0, 0, 1 },
				{ 0, 0, 2, 0, 0, 0, 4, 0, 0 },
				{ 0, 6, 1, 0, 0, 0, 7, 9, 0 },

				{ 6, 0, 0, 2, 4, 7, 0, 0, 5 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 2, 0, 0, 8, 3, 6, 0, 0, 4 },

				{ 0, 4, 6, 0, 0, 0, 2, 3, 0 },
				{ 0, 0, 9, 0, 0, 0, 6, 0, 0 },
				{ 5, 0, 0, 3, 0, 9, 0, 0, 8 }
		}, {
				{ 0, 0, 0, 0, 1, 0, 0, 0, 0 },
				{ 3, 0, 1, 4, 0, 0, 8, 6, 0 },
				{ 9, 0, 0, 5, 0, 0, 2, 0, 0 },

				{ 7, 0, 0, 1, 6, 0, 0, 0, 0 },
				{ 0, 2, 0, 8, 0, 5, 0, 1, 0 },
				{ 0, 0, 0, 0, 9, 7, 0, 0, 4 },

				{ 0, 0, 3, 0, 0, 4, 0, 0, 6 },
				{ 0, 4, 8, 0, 0, 6, 9, 0, 7 },
				{ 0, 0, 0, 0, 8, 0, 0, 0, 0 }
		}, {	// Fiendish puzzle April 21, 2005 Times London
				{ 0, 0, 4, 0, 0, 3, 0, 7, 0 },
				{ 0, 8, 0, 0, 7, 0, 0, 0, 0 },
				{ 0, 7, 0, 0, 0, 8, 2, 0, 5 },

				{ 4, 0, 0, 0, 0, 0, 3, 1, 0 },
				{ 9, 0, 0, 0, 0, 0, 0, 0, 8 },
				{ 0, 1, 5, 0, 0, 0, 0, 0, 4 },

				{ 1, 0, 6, 9, 0, 0, 0, 3, 0 },
				{ 0, 0, 0, 0, 2, 0, 0, 6, 0 },
				{ 0, 2, 0, 4, 0, 0, 5, 0, 0 }
		}, {	// This one requires search
				{ 0, 4, 3, 0, 8, 0, 2, 5, 0 },
				{ 6, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 1, 0, 9, 4 },

				{ 9, 0, 0, 0, 0, 4, 0, 7, 0 },
				{ 0, 0, 0, 6, 0, 8, 0, 0, 0 },
				{ 0, 1, 0, 2, 0, 0, 0, 0, 3 },

				{ 8, 2, 0, 5, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 5 },
				{ 0, 3, 4, 0, 9, 0, 7, 1, 0 }
		}, {	// Hard one from http://www.cs.mu.oz.au/671/proj3/node5.html
				{ 0, 0, 0, 0, 0, 3, 0, 6, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 1, 0 },
				{ 0, 9, 7, 5, 0, 0, 0, 8, 0 },

				{ 0, 0, 0, 0, 9, 0, 2, 0, 0 },
				{ 0, 0, 8, 0, 7, 0, 4, 0, 0 },
				{ 0, 0, 3, 0, 6, 0, 0, 0, 0 },

				{ 0, 1, 0, 0, 0, 2, 8, 9, 0 },
				{ 0, 4, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 5, 0, 1, 0, 0, 0, 0, 0 }
		}, { // Puzzle 1 from http://www.sudoku.org.uk/bifurcation.htm
				{ 1, 0, 0, 9, 0, 7, 0, 0, 3 },
				{ 0, 8, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 0, 9, 0, 0, 0, 6, 0, 0 },
				{ 0, 0, 7, 2, 0, 9, 4, 0, 0 },
				{ 4, 1, 0, 0, 0, 0, 0, 9, 5 },
				{ 0, 0, 8, 5, 0, 4, 3, 0, 0 },
				{ 0, 0, 3, 0, 0, 0, 7, 0, 0 },
				{ 0, 5, 0, 0, 0, 0, 0, 4, 0 },
				{ 2, 0, 0, 8, 0, 6, 0, 0, 9 }
		}, { // Puzzle 2 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 0, 3, 0, 2, 0, 0, 0 },
				{ 0, 5, 0, 7, 9, 8, 0, 3, 0 },
				{ 0, 0, 7, 0, 0, 0, 8, 0, 0 },
				{ 0, 0, 8, 6, 0, 7, 3, 0, 0 },
				{ 0, 7, 0, 0, 0, 0, 0, 6, 0 },
				{ 0, 0, 3, 5, 0, 4, 1, 0, 0 },
				{ 0, 0, 5, 0, 0, 0, 6, 0, 0 },
				{ 0, 2, 0, 4, 1, 9, 0, 5, 0 },
				{ 0, 0, 0, 8, 0, 6, 0, 0, 0 }
		}, { // Puzzle 3 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 0, 8, 0, 0, 0, 0, 6 },
				{ 0, 0, 1, 6, 2, 0, 4, 3, 0 },
				{ 4, 0, 0, 0, 7, 1, 0, 0, 2 },
				{ 0, 0, 7, 2, 0, 0, 0, 8, 0 },
				{ 0, 0, 0, 0, 1, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0, 6, 2, 0, 0 },
				{ 1, 0, 0, 7, 3, 0, 0, 0, 4 },
				{ 0, 2, 6, 0, 4, 8, 1, 0, 0 },
				{ 3, 0, 0, 0, 0, 5, 0, 0, 0 }
		}, { // Puzzle 4 from http://www.sudoku.org.uk/bifurcation.htm
				{ 3, 0, 5, 0, 0, 4, 0, 7, 0 },
				{ 0, 7, 0, 0, 0, 0, 0, 0, 1 },
				{ 0, 4, 0, 9, 0, 0, 0, 3, 0 },
				{ 4, 0, 0, 0, 5, 1, 0, 0, 6 },
				{ 0, 9, 0, 0, 0, 0, 0, 4, 0 },
				{ 2, 0, 0, 8, 4, 0, 0, 0, 7 },
				{ 0, 2, 0, 0, 0, 7, 0, 6, 0 },
				{ 8, 0, 0, 0, 0, 0, 0, 9, 0 },
				{ 0, 6, 0, 4, 0, 0, 2, 0, 8 }
		}, { // Puzzle 5 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 0, 7, 0, 0, 3, 0, 0 },
				{ 0, 6, 0, 0, 0, 0, 5, 7, 0 },
				{ 0, 7, 3, 8, 0, 0, 4, 1, 0 },
				{ 0, 0, 9, 2, 8, 0, 0, 0, 0 },
				{ 5, 0, 0, 0, 0, 0, 0, 0, 9 },
				{ 0, 0, 0, 0, 9, 3, 6, 0, 0 },
				{ 0, 9, 8, 0, 0, 7, 1, 5, 0 },
				{ 0, 5, 4, 0, 0, 0, 0, 6, 0 },
				{ 0, 0, 1, 0, 0, 9, 0, 0, 0 }
		}, { // Puzzle 6 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 0, 6, 0, 0, 0, 0, 4 },
				{ 0, 3, 0, 0, 9, 0, 0, 2, 0 },
				{ 0, 6, 0, 8, 0, 0, 7, 0, 0 },
				{ 0, 0, 5, 0, 6, 0, 0, 0, 1 },
				{ 6, 7, 0, 3, 0, 1, 0, 5, 8 },
				{ 9, 0, 0, 0, 5, 0, 4, 0, 0 },
				{ 0, 0, 6, 0, 0, 3, 0, 9, 0 },
				{ 0, 1, 0, 0, 8, 0, 0, 6, 0 },
				{ 2, 0, 0, 0, 0, 6, 0, 0, 0 }
		}, { // Puzzle 7 from http://www.sudoku.org.uk/bifurcation.htm
				{ 8, 0, 0, 0, 0, 1, 0, 4, 0 },
				{ 2, 0, 6, 0, 9, 0, 0, 1, 0 },
				{ 0, 0, 9, 0, 0, 6, 0, 8, 0 },
				{ 1, 2, 4, 0, 0, 0, 0, 0, 9 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 9, 0, 0, 0, 0, 0, 8, 2, 4 },
				{ 0, 5, 0, 4, 0, 0, 1, 0, 0 },
				{ 0, 8, 0, 0, 7, 0, 2, 0, 5 },
				{ 0, 9, 0, 5, 0, 0, 0, 0, 7 }
		}, { // Puzzle 8 from http://www.sudoku.org.uk/bifurcation.htm
				{ 6, 5, 2, 0, 4, 8, 0, 0, 7 },
				{ 0, 7, 0, 2, 0, 5, 4, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 6, 4, 1, 0, 0, 0, 7, 0 },
				{ 0, 0, 0, 0, 8, 0, 0, 0, 0 },
				{ 0, 8, 0, 0, 0, 4, 5, 6, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 8, 6, 0, 7, 0, 2, 0 },
				{ 2, 0, 0, 8, 9, 0, 7, 5, 1 }
		}, { // Puzzle 9 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 6, 0, 0, 2, 0, 0, 9 },
				{ 1, 0, 0, 5, 0, 0, 0, 2, 0 },
				{ 0, 4, 7, 3, 0, 6, 0, 0, 1 },
				{ 0, 0, 0, 0, 0, 8, 0, 4, 0 },
				{ 0, 3, 0, 0, 0, 0, 0, 7, 0 },
				{ 0, 1, 0, 6, 0, 0, 0, 0, 0 },
				{ 4, 0, 0, 8, 0, 3, 2, 1, 0 },
				{ 0, 6, 0, 0, 0, 1, 0, 0, 4 },
				{ 3, 0, 0, 4, 0, 0, 9, 0, 0 }
		}, { // Puzzle 10 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 4, 0, 5, 0, 9, 0, 0 },
				{ 0, 0, 0, 0, 7, 0, 0, 0, 6 },
				{ 3, 7, 0, 0, 0, 0, 0, 0, 2 },
				{ 0, 0, 9, 5, 0, 0, 0, 8, 0 },
				{ 0, 0, 1, 2, 0, 4, 3, 0, 0 },
				{ 0, 6, 0, 0, 0, 9, 2, 0, 0 },
				{ 2, 0, 0, 0, 0, 0, 0, 9, 3 },
				{ 1, 0, 0, 0, 4, 0, 0, 0, 0 },
				{ 0, 0, 6, 0, 2, 0, 7, 0, 0 }
		}, { // Puzzle 11 from http://www.sudoku.org.uk/bifurcation.htm
				{ 0, 0, 0, 0, 3, 0, 7, 9, 0 },
				{ 3, 0, 0, 0, 0, 0, 0, 0, 5 },
				{ 0, 0, 0, 4, 0, 7, 3, 0, 6 },
				{ 0, 5, 3, 0, 9, 4, 0, 7, 0 },
				{ 0, 0, 0, 0, 7, 0, 0, 0, 0 },
				{ 0, 1, 0, 8, 2, 0, 6, 4, 0 },
				{ 7, 0, 1, 9, 0, 8, 0, 0, 0 },
				{ 8, 0, 0, 0, 0, 0, 0, 0, 1 },
				{ 0, 9, 4, 0, 1, 0, 0, 0, 0 }
		}, { // From http://www.sudoku.org.uk/discus/messages/29/51.html?1131034031
				{ 2, 5, 8, 1, 0, 4, 0, 3, 7 },
				{ 9, 3, 6, 8, 2, 7, 5, 1, 4 },
				{ 4, 7, 1, 5, 3, 0, 2, 8, 0 },

				{ 7, 1, 5, 2, 0, 3, 0, 4, 0 },
				{ 8, 4, 9, 6, 7, 5, 3, 2, 1 },
				{ 3, 6, 2, 4, 1, 0, 0, 7, 5 },

				{ 1, 2, 4, 9, 0, 0, 7, 5, 3 },
				{ 5, 9, 3, 7, 4, 2, 1, 6, 8 },
				{ 6, 8, 7, 3, 5, 1, 4, 9, 2 }
		}
};

class Sudoku : public Script {
protected:
	IntVarArray x;
public:
	Sudoku(const SizeOptions& opt)
		: Script(opt), x(*this, MSIZE*MSIZE, 1, 9) {

		Matrix<IntVarArray> m(x, MSIZE, MSIZE);

		// Constraints for rows and columns
		for (int i = 0; i<MSIZE; i++) {
			distinct(*this, m.row(i), opt.icl());
			distinct(*this, m.col(i), opt.icl());
		}

		// Constraints for squares
		for (int i = 0; i<MSIZE; i += SQRSIZE) {
			for (int j = 0; j<MSIZE; j += SQRSIZE) {
				distinct(*this, m.slice(i, i + SQRSIZE, j, j + SQRSIZE), opt.icl());
			}
		}

		// Fill-in predefined fields
		for (int i = 0; i<MSIZE; i++)
			for (int j = 0; j<MSIZE; j++)
				if (examples[PROBLEM_NUM][j][i] != 0)
					rel(*this, m(i, j), IRT_EQ, examples[PROBLEM_NUM][j][i]);

		branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());

	}

	/// Constructor for cloning \a s
	Sudoku(bool share, Sudoku& s) : Script(share, s) {
		x.update(*this, share, s.x);
	}

	/// Perform copying during cloning
	virtual Space*
		copy(bool share) {
		return new Sudoku(share, *this);
	}

	/// Print solution
	virtual void
		print(std::ostream& os) const {
		os << "  ";
		for (int i = 0; i<MSIZE*MSIZE; i++) {
			if (x[i].assigned()) {
				os << x[i] << " ";
			}
			else
				os << ". ";
			if ((i + 1) % (MSIZE) == 0)
				os << std::endl << "  ";
		}
		os << std::endl;
	}
};

int main(int argc, char* argv[]) {
	SizeOptions opt("Sudoku");
	opt.icl(ICL_DOM);
	opt.solutions(1);
	Script::run<Sudoku, DFS, SizeOptions>(opt);
	system("pause");
	return 0;
}