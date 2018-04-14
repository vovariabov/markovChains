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
		void chain::print(ostream& = cout, int = 3);
		chain chain::operator* (const chain&) const;
		chain chain::pov(int);
		vector<double> chain::apply(vector<double>);
};
chain::chain(int N)
{
	size = N;
	vector <vector <double> > result(N, vector<double>(N, 0));
	Matrix = result;
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
		if (int(temp[1]) != -51) strings.push_back(temp);		
	}
	fcin.close();
	int len = strings.size();
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

vector<double> chain::apply(vector<double> v)
{
	vector <double> result;
	if (v.size() != size) return result;
	for (int i = 0; i < size; i++)
	{
		double temp = 0;
		for (int j = 0; j < size; j++)
		{
			temp += Matrix[j][i] * v[j];
		}
		result.push_back(temp);
	}
	return result;
}
double prob(chain MC, int start, int finish, int step) 
//вероятность попасть из позиции start в позицию finist на шаге step
{
	return (MC.pov(step)).Matrix[start][finish];
}

vector <double> probvector(chain MC, vector <double> v, int step) 
//по вектору распредиления вероятностей v в начальном положении получает вектор распредиления вероятностей на шаге step;
{
	return (MC.pov(step)).apply(v);
}

double expectedvalue(chain MC, vector <double> v, int step)
//матожидание положения на шаге step о вектору распредиления вероятностей v в начальном положении;
{
	double result = 0;
	vector <double> probvec = probvector(MC, v, step);
	for (int i = 0; i < probvec.size(); i++)
	{
		result += (i + 1)*probvec[i];
	}
	return result;
}

bool reachable(chain MC, int start, int finish, bool essential = false)
//проверяет достижимость finish из start
{
	int size = MC.size;
	chain M(size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			M.Matrix[i][j] = double(MC.Matrix[i][j] > 0 || (i == j && !essential));
		}
	}
	return (prob(M, start, finish, size) > 0);
}

vector <int> reachableset(chain MC, int start)
//возвращает достижимое множестов положения start
{
	int size = MC.size;
	chain M(size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			M.Matrix[i][j] = double(MC.Matrix[i][j] > 0 || i == j);
		}
	}
	M.print();
	vector <double> v(size, 0);
	vector <int> result(0);
	v[start] = 1;
	v = M.apply(v);
	for (int i = 0; i < size; i++)
	{
		if (v[i] > 0)
			result.push_back(i);
	}
	return result;
}

void printset(vector<int> v, ostream& c, string left = "", string right = "")
{
	cout << left << " ";
	int size = v.size();
	for (int i = 0; i < size; i++)
	{
		c << v[i] + 1 << ' ';
	}
	cout << right;
}

bool essential(chain MC, int start)
//проверяте существенность состояния start
{
	return reachable(MC, start, start, true);
}

bool interconnected(chain MC, int s1, int s2)
// проверяет сообщаются ли состояния s1 и s2
{
	int size = MC.size;
	chain M(size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			M.Matrix[i][j] = double(MC.Matrix[i][j] > 0 || i == j);
		}
	}
	M = M.pov(size);
	return (M.Matrix[s1][s2] > 0 && M.Matrix[s2][s1] > 0);
}

vector <vector <int> > interconnectedclasses(chain MC)
//возвращает классы сообщаемости цепи MC
{
	int size = MC.size;
	chain M(size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			M.Matrix[i][j] = double(MC.Matrix[i][j] > 0 || i == j);
		}
	}
	M = M.pov(size);
	vector <bool> marked(size, false);
	vector <vector <int> > results;
	int j = 0;
	bool done = false;
	while (!done)
	{
		vector <int> intclass;
		for (int i = 0; i < size; i++)
		{
			if ((M.Matrix[i][j] > 0) && (M.Matrix[j][i] > 0)) {
				intclass.push_back(i);
				marked[i] = true;
			}
		}
		results.push_back(intclass);
		done = true;
		for (int i = 0; i < size; i++)
		{
			if (!marked[i]) {
				j = i;
				done = false;
				break;
			}
		}
	}
	return results;
}

void printinterconnectedclasses(chain MC, ostream& c) {
	vector <vector <int> > results = interconnectedclasses(MC);
	int number = results.size();
	for (int i = 0; i < number; i++)
	{
		printset(results[i], c, "CLASS : ", "\n");
	}
}

int main() 
{
	chain MC = fetch("data.txt");
	MC.print();
	cout << endl;
	vector <double> v;
	int start, finish;
	printinterconnectedclasses(MC, cout);
	system("pause");
}