#pragma once
typedef std::vector <double> row;
typedef std::vector< row > matrix;

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