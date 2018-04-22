#include <iostream>
#include <vector>


typedef std::vector <double> row;
typedef std::vector< row > matrix;

using namespace std;
class Package
{
public:
	matrix Package::Add(matrix, matrix);
	void Package::Swap(matrix&, int, int);
	int Package::Rank(matrix, double = 0);
	matrix Package::Mult(matrix, matrix);
	matrix Package::Pov(matrix, int);
	const matrix Package::PrepareIdentity(int, int = 1);
	row Package::Apply(matrix, row);
private:

};

const matrix Package::PrepareIdentity(int size, int lambda)
{
	matrix I(size, row(size, 0));
	for (int i = 0; i < size; i++)
	{
		I[i][i] = lambda;
	}
	return I;
}

matrix Package::Add(matrix M, matrix N) {
	matrix result(M.size(), row(M.size(), 0));
	for (int i = 0; i < M.size(); i++) {
		for (int j = 0; j < M.size(); j++) {
			result[i][j] = M[i][j] + N[i][j];
		}
	}
	return result;
}

void Package::Swap(matrix& M, int i, int j) {
	for (int k = 0; k < M.size(); k++) {
		std::swap(M[i][k], M[j][k]);
	}
}


matrix Package::Mult(matrix M, matrix N)
{
	matrix result(M.size(), row(M.size(), 0));
	for (int i = 0; i < M.size(); i++) {
		for (int j = 0; j < M.size(); j++) {
			for (int k = 0; k < M.size(); k++) {
				result[i][j] += M[i][k] + N[k][j];
			}
		}
	}
	return result;
}

matrix Package::Pov(matrix M, int pov) {
	matrix multiplicant = M, result = Package::PrepareIdentity(M.size());
	
	while (pov > 0) {
		if (pov % 2 == 0)
		{
			multiplicant = Package::Mult(multiplicant, multiplicant);
			pov /= 2;
		}
		else {
			result = Package::Mult(result, multiplicant);
			pov--;
		}
	}
	return result;
}

row Package::Apply(matrix M, row v) {
	row result;
	for (int i = 0; i < M.size(); i++)
	{
		double temp = 0;
		for (int j = 0; j < M.size(); j++)
		{
			temp += M[j][i] * v[j];
		}
		result.push_back(temp);
	}
	return result;
}

void swap(matrix& mat, int row1, int row2,
	int col)
{
	for (int i = 0; i < col; i++)
	{
		int temp = mat[row1][i];
		mat[row1][i] = mat[row2][i];
		mat[row2][i] = temp;
	}
}

int Package::Rank(matrix mat, double epsilon) {
	int rank = mat.size();

	for (int row = 0; row < rank; row++)
	{
		if (mat[row][row])
		{
			for (int col = 0; col < mat.size(); col++)
			{
				if (col != row)
				{
					double mult = mat[col][row] / mat[row][row];
					for (int i = 0; i < rank; i++)
						mat[col][i] -= mult * mat[row][i];
				}
			}
		}

		else
		{
			bool reduce = true;
			
			for (int i = row + 1; i < mat.size(); i++)
			{
				if (abs(mat[i][row]) > epsilon)
				{
					Package::Swap(mat, i, row);
					reduce = false;
					break;
				}
			}

			if (reduce)
			{
				rank--;
				for (int i = 0; i < mat.size(); i++)
					mat[i][row] = mat[i][rank];
			}
			row--;
		}
	}
	return rank;
}