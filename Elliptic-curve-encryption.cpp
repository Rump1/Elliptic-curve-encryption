#include <iostream>
#include <unordered_map>
#include <ctime>

using namespace std;

//кривая Е41(1.3)
int p = 41;
int a = 1;
int b = 3;

unordered_map <char, pair<int, int>> dictionary = //Алфавит
{
{'h',make_pair(1,13)},
{'e',make_pair(1,28)},
{'l',make_pair(7,5)},
{'o',make_pair(7,36)},
{'w',make_pair(12,12)},
{'r',make_pair(12,29)},
{'j',make_pair(15,20)},
{'d',make_pair(15,21)},
{'a',make_pair(23,4)},
{'s',make_pair(23,37)},
{'f',make_pair(27,19)},
{'g',make_pair(27,22)},
{'*', make_pair(0,0)},
};

int invert(int a, int mod) // Алгоритм поиска обратного по модулю
{
	if (a == 1)
		return 1;
	return (1 - 1 * invert(mod % a, a) * mod) / a + mod;
}

pair<int, int> add(pair<int, int> P, pair<int, int> Q) // Сложение двух точек
{
	pair<int, int> res;

	//замена отрицательных координат
	if (P.second < 0) P.second += p;
	if (Q.second < 0) Q.second += p;

	if (((P.second + Q.second) == p) && (P.first == Q.first))  //сложение симметричных точек
	{
		res.first = 0;
		res.second = 0;
	}
	//сложение с точкой О
	else if ((P.first == 0) && (P.second == 0))
	{
		res.first = Q.first;
		res.second = Q.second;
	}
	else if ((Q.first == 0) && (Q.second == 0))
	{
		res.first = P.first;
		res.second = P.second;
	}
	else {
		int m;
		if ((P.first == Q.first) && (P.second == Q.second))						//сложение одиннаковых точек
			m = ((3 * P.first * P.first + a) * invert((2 * P.second), p)) % p;
		else
		{
			int sum = (P.first - Q.first);     //различные точки
			if (sum < 0) sum += p;
			m = ((P.second - Q.second) * invert(sum, p)) % p;
		}
		res.first = (m * m - P.first - Q.first) % p;
		res.second = (P.second + m * (res.first - P.first)) % p;

		res.second *= -1;
	}

	//замена отрицательных координат
	if (res.first < 0) res.first += p;
	if (res.second < 0) res.second += p;

	return res;
}


char search_key_by_value(pair<int, int> value)       //сопоставление точки и буквы
{
	unordered_map <char, pair<int, int>> ::iterator it = dictionary.begin();
	for (it; it != dictionary.end(); it++)
	{
		if (it->second == value)
			return it->first;
	}
}

pair<int, int> mult(pair<int, int> P, int n) // Умножение точки на число
{
	pair<int, int> res;
	res.first = P.first;
	res.second = P.second;
	for (int i = 0; i < n - 1; i++)
	{
		res = add(res, P);
	}
	return res;
}

pair<pair<int, int>, pair<int, int>> point_encrypt(pair<int, int> G, pair<int, int> Pm, pair<int, int> foreign_opened_key) // Шифрование точки
{
	int k = 2 + rand() % 9;
	pair<pair<int, int>, pair<int, int>> res;
	res.first = mult(G, k);
	res.second = add(Pm, mult(foreign_opened_key, k));

	return res;
}

pair<int, int> point_decrypt(pair<pair<int, int>, pair<int, int>> encrypted_point, int my_closed_key) // Расшифрование точки
{
	pair<int, int> res;
	pair<int, int> temp;
	temp = mult(encrypted_point.first, my_closed_key);
	temp.second = -temp.second + p;
	res = add(encrypted_point.second, temp);
	return res;
}

string encryption(string input, pair<int, int> G, pair<int, int> foreign_opened_key) // Шифрование текста
{
	string result = "";

	for (int i = 0; i < input.length(); i++)
	{
		unordered_map<char, pair<int, int>> ::iterator it;
		it = dictionary.find(input[i]); // Ставим итератор it1 на ячейку словаря1 с ключем input[i]
		pair<pair<int, int>, pair<int, int>> pair_of_points = point_encrypt(G, it->second, foreign_opened_key); // Шифруем точку, получаем две
		result += search_key_by_value(pair_of_points.first);
		result += search_key_by_value(pair_of_points.second);
	}
	return result;
}

string decryption(string input, int my_closed_key)   //Расшифрование текста
{
	string result = "";
	for (int i = 0; i < input.length(); i += 2)
	{
		pair<pair<int, int>, pair<int, int>> pair_of_points;
		unordered_map<char, pair<int, int>> ::iterator it;
		it = dictionary.find(input[i]);
		pair_of_points.first = it->second;
		it = dictionary.find(input[i + 1]);
		pair_of_points.second = it->second;
		pair<int, int> decrypted_point = point_decrypt(pair_of_points, my_closed_key);
		result += search_key_by_value(decrypted_point);
	}
	return result;
}

int main()
{
	setlocale(LC_ALL, "ru");
	srand(time(0));
	pair<int, int> G = make_pair(12, 12);

	int my_closed_key = 17;         //мой закрытый
	pair<int, int> my_opened_key = mult(G, my_closed_key); // (1,13) мой открытый
	pair<int, int> foreign_opened_key = make_pair(27, 22);  //чужой открытый


	//int my_closed_key = 7;         //мой закрытый
	//pair<int, int> my_opened_key = mult(G, my_closed_key); // (27,22) мой открытый
	//pair<int, int> foreign_opened_key = make_pair(1, 13);  //чужой открытый
	//

	int mode;
	cout << "Если хотите зашифровать сообщение, введите 1. Расшифровать - 2" << endl;
	cin >> mode;
	if (mode == 1)
	{
		string input;
		cout << "Введите текст для шифрования: ";
		cin >> input;
		string encrypted = encryption(input, G, foreign_opened_key);
		cout << "Зашифрованное сообщение: " << encrypted << endl;
	}
	else if (mode == 2)
	{
		string input;
		cout << "Введите текст для расшифрования: ";
		cin >> input;
		cout << "Расшифрованное сообщение: " << decryption(input, my_closed_key) << endl;
	}
	else
	{
		cout << "Перезапустите программу и введите 1 или 2";
	}

}