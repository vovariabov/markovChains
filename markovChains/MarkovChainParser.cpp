#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <string>
#include <locale.h>
#include <fstream>
#include <vector>
#include "MatrixMath.h"

using namespace std;

Package package;

class chain {
	public:
		matrix	Matrix;
		int							size;
		chain::chain(int = 0);
		void chain::print(ostream& = cout, int = 3);
		chain chain::operator* (const chain&) const;
		chain chain::pov(int);
		row chain::apply(row);
		int chain::rank(int = 0);
};
chain::chain(int N)
{
	size = N;
	matrix result(N, row(N, 0));
	Matrix = result;
}

void chain::print(ostream& c, int precision) {
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
	//
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
			chain.Matrix[i][index] = probab;
		}
	}
	return chain;
}

chain chain::operator* (const chain& c) const {
	//оператор умножения матриц состояний двух цепей.
	if (size != c.size) return chain();
	chain result(size);
	result.Matrix = package.Mult(Matrix, c.Matrix);
	return result;
}

chain chain::pov(int pov) {
	//возводит матрицу переходов цепи в степерь pov.
	chain result(size);
	result.Matrix = package.Pov(Matrix, pov);
	return result;
}

row chain::apply(row v) {
	//умножает вектор v на матрицу переходов цепи.
	if (v.size() != size) return row();
	row result = package.Apply(Matrix, v);

}

int chain::rank(int epsilon) {
	return package.Rank(Matrix, epsilon);
}
double prob(chain MC, int start, int finish, int step) {
	//вероятность попасть из позиции start в позицию finist на шаге step.
	return (MC.pov(step)).Matrix[start][finish];
}

row probvector(chain MC, row v, int step) 
//по вектору распредиления вероятностей v в начальном положении получает вектор распредиления вероятностей на шаге step;
{
	return (MC.pov(step)).apply(v);
}

double expectedvalue(chain MC, row v, int step) {
	//матожидание положения на шаге step о вектору распредиления вероятностей v в начальном положении.
	double result = 0;
	row probvec = probvector(MC, v, step);
	for (int i = 0; i < probvec.size(); i++)
	{
		result += i * probvec[i];
	}
	return result;
}

bool reachable(chain MC, int start, int finish, bool essential = false) {
	//проверяет достижимость finish из start.
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

vector <int> reachableset(chain MC, int start) {
	//возвращает достижимое множестов положения start.
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
	row v(size, 0);
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

void printset(vector<int> v, ostream& c, string left = "", string right = "") {
	//выводит вектор v в стрим c с левой и правым и правым идентификатором left и rightю.
	cout << left << " ";
	int size = v.size();
	for (int i = 0; i < size; i++)
	{
		c << v[i] << ' ';
	}
	cout << right;
}

bool essential(chain MC, int start)
//проверяте существенность состояния start
{
	return reachable(MC, start, start, true);
}

bool interconnected(chain MC, int s1, int s2) {
	//проверяет сообщаются ли состояния s1 и s2.
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

vector <vector <int> > interconnectedclasses(chain MC) {
	//возвращает классы сообщаемости цепи MC.
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
	//выводит список сообщающихся классов цепи MC в поток c.
	vector <vector <int> > results = interconnectedclasses(MC);
	int number = results.size();
	for (int i = 0; i < number; i++)
	{
		printset(results[i], c, "CLASS : ", "\n");
	}
}


//-------------------------------------temporary, algebrainc solution is required----------------------------------------------------------
int decomposible(chain MC, ostream& c) {
	//возвращает количество сообщающихся классов ЬС в поток c.
	vector <vector <int> > results = interconnectedclasses(MC);
	int number = results.size();
	return number;
}

std::pair< vector <int>, vector <int> > essentialStates(chain MC) {
	//возвращает вектор существенных (.first) и несущественных (.second) состояний цепи MC
	std::pair< vector <int>, vector <int> > result;
	for (int i = 0; i < MC.size; i++)
	{
		essential(MC, i) ? result.first.push_back(i) : result.second.push_back(i);
	}
	return result;
}
//-----------------------------------------------------------------------------------------------------------------------------------------

//В4
bool returnState(chain MC, int x, int INFepsilon) {
	//определяет возвратность состояния x цепи MC с точностью до epsilon
	const int INF = 1000000;
	row temp(MC.size, 0);
	temp[x] = 1;
	long long res = 0;
	for (int i = 0; i < INF; i++) {
		temp = MC.apply(temp);
		res += temp[x];
		if (x > INFepsilon) return true;
	}
	return false;
}

vector <int> allReturnStates(chain MC, int INFepsilon) {
	//возвращает вектор всех возвратных состояний цепи MC
	vector<int> states;
	for (int i = 0; i < MC.size; i++)
	{
		if (returnState(MC, i, INFepsilon) ) states.push_back(i);
	}
	return states;
}


int main() 
{
	chain MC = fetch("data.txt");
	MC.print();
	
	
	cout << endl;
	row v;
	int start, finish;
	
	cout << MC.rank() << " " ;

	system("pause");
	return 0;
}