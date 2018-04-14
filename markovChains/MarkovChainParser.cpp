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
		chain::chain(int);
		void chain::print(ostream&, int);
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

int main()
{
	chain MC = fetch("data.txt");
	MC.print(cout, 2);
	system("pause");
}