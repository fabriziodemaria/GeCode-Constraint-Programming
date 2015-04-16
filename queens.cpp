#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

const int MSIZE = 15;

class Queens : public Script {
protected:
	IntVarArray x;
public:
	Queens(const SizeOptions& opt)
		: Script(opt), x(*this, MSIZE*MSIZE, 0, 1) {

		Matrix<IntVarArray> m(x, MSIZE, MSIZE);

		//Check rows and columns
		for (int i = 0; i < MSIZE; i++) {
			linear(*this, m.col(i), IRT_EQ, 1);
			linear(*this, m.row(i), IRT_EQ, 1);
		}


		//Diagonal check 1
		for (int i = 0; i < MSIZE; i++){
			IntVarArray d(*this, MSIZE - i);
			for (int j = 0; j < MSIZE - i; j++){
				d[j] = m(i + j,j);
			}
			linear(*this, d, IRT_LQ, 1);
		}

		//Diagonal check 2
		for (int i = 0; i < MSIZE; i++){
			IntVarArray d(*this, MSIZE - i);
			for (int j = 0; j < MSIZE - i; j++){
				d[j] = m(j,j+i);
			}
			linear(*this, d, IRT_LQ, 1);
		}

		//Diagonal check 3
		for (int i = 0; i < MSIZE; i++){
			IntVarArray d(*this, MSIZE - i);
			for (int j = 0; j < MSIZE - i; j++){
				d[j] = m(j+i, MSIZE-1-j);
			}
			linear(*this, d, IRT_LQ, 1);
		}
		
		//Diagonal check 4
		for (int i = 0; i < MSIZE; i++){
			IntVarArray d(*this, i+1);
			for (int j = 0; j <= i; j++){
				d[j] = m(i-j, j);
			}
			linear(*this, d, IRT_LQ, 1);
		}

		Rnd r(1U);
		//branch(*this, x, INT_VAR_RND(r), INT_VAL_MAX());
		branch(*this, x, INT_VAR_NONE(), INT_VAL_MAX());
	}

	/// Constructor  for cloning \a s
	Queens(bool share, Queens& s) : Script(share, s) {
		x.update(*this, share, s.x);
	}

	/// Perform copying during cloning
	virtual Space*
		copy(bool share) {
		return new Queens(share, *this);
	}

	/// Print solution
	virtual void
		print(std::ostream& os) const {
		os << "Board\t";
		for (int i = 0; i < x.size(); i++) {
			os << x[i] << "";
			if ((i + 1) % MSIZE == 0)
				os << std::endl << "\t";
		}
		os << std::endl;
	}
};


int
main(int argc, char* argv[]) {
	SizeOptions opt("Queens");
	opt.parse(argc, argv);
	Script::run<Queens, DFS, SizeOptions>(opt);
	system("pause");
	return 0;
}

