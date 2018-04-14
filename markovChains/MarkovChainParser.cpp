#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <string>
#include <locale.h>
#include <fstream>
#include <vector>

using namespace std;

class chain {
	public:
		vector < vector <double> >	Matrix;
		int							size;
		chain::chain(int = 0);
		void chain::print(ostream& = cout, int = 10);
		chain chain::operator* (const chain&) const;
		chain chain::pov(int);
};
chain::chain(int N)
{
	size = N;
	vector <double> temp;
	for (int j = 0; j < N; j++)
	{
		temp.push_back(0);
	}
	for (int j = 0; j < N; j++)
	{
		Matrix.push_back(temp);
	}
}

void chain::print(ostream& c, int precision)
{
	c.precision(precision);
	c << fixed;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{

			c << left << Matrix[i][j] << " " ;
		}
		c << endl;
	}
}

chain fetch(string path) {
	setlocale(LC_NUMERIC, "French_Canada.1252");
	ifstream fcin(path);
	vector <char*> strings;
	
	while (fcin) {
		char* temp = new char[256];
		fcin.getline(temp, 256);
		strings.push_back(temp);
	}
	fcin.close();
	int len = strings.size() - 1;
	double probab;
	int index, offset;
	chain chain(len);
	for (int i = 0; i < len; i++) {
		int c;
		while (sscanf(strings[i], "%d%n", &index, &offset) == 1)
		{
			strings[i] += offset;
			sscanf(strings[i], "%lf%n", &probab, &offset);
			strings[i] += offset;
			chain.Matrix[i][index-1] = probab;
		}
	}
	return chain;
}

chain chain::operator* (const chain& c) const {
	if (size != c.size) return chain();
	chain result(size);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				result.Matrix[i][j] += Matrix[i][k] * c.Matrix[k][j];
			}
		}
	}
	return result;
}

chain chain::pov(int pov)
{
	chain result(size);
	result.Matrix = this->Matrix;
	chain multiplicant(size);
	multiplicant.Matrix = this->Matrix;
	while (pov > 0) {
		if (pov % 2 == 0)
		{
			multiplicant = multiplicant*multiplicant;
			pov /= 2;
		}
		else {
			result = result*multiplicant;
			pov--;
		}
	}
	return result;
}

double probability(chain MC, int start, int finish, int steps)
{
	return (MC.pov(steps)).Matrix[start][finish];
}
int main()
{
	chain MC = fetch("data.txt");
	MC.print();
	cout << endl;
	int start, finish, steps;
	cin >> start >> finish >> steps;
	cout << probability(MC, start-1, finish-1, steps) << endl;
	system("pause");
}