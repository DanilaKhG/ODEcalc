#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <iostream>
using namespace std;
double gX, gY, h, gX1, gY1;
int n;
double pi = 3.141592;
double e = 2.71828;
//double gX = 1;
struct Expression {
	Expression(std::string token) : token(token) {} //Конструктор для чисел
	Expression(std::string token, Expression a) : token(token), args{ a } {} //Конструктор для унарных операций
	Expression(std::string token, Expression a, Expression b) : token(token), args{ a, b } {} //Конструктор бинарных операций
	std::string token;
	std::vector<Expression> args;
};
class Parser {
public:
	explicit Parser(const char* input) : input(input) {} //Конструктор - строка с выражением
	Expression parse();
private:
	std::string parse_token(); //Парсит только один токен
	Expression parse_simple_expression(); //Парсит простое выражение
	Expression parse_binary_expression(int min_priority); //Парсит бинарное выражение
	const char* input; //Правая часть строки, которую ещё не парсили
};

std::string Parser::parse_token() {
	while (std::isspace(*input)) ++input; //Пропускаем любое кол-во пробелов перед токеном
	if (std::isdigit(*input)) {
		std::string number;
		while (std::isdigit(*input) || *input == '.') number.push_back(*input++);
		return number;
	}
	//Список токенов операций + скобок
	static const std::string tokens[] =
	{ "+", "-", "**", "^", "*", "/", "mod", "abs", "sin", "cos", "tg", "ctg", "sec", "csec", "ln", "(", ")", ".", "x", "y", "X", "Y", "e", "pi" };
	for (auto& t : tokens) {
		if (std::strncmp(input, t.c_str(), t.size()) == 0) {
			input += t.size();
			return t;
		}
	}
	return "";
}

Expression Parser::parse_simple_expression() {
	
	auto token = parse_token();
	if (token.empty()) throw std::runtime_error("Invalid input");
	//Проверяем строку на открытую и закрутую скобку
	if (token == "x" || token == "X") {
		token = to_string(gX);
		return Expression(token);
	}
	if (token == "y" || token == "Y") {
		token = to_string(gY);
		return Expression(token);
	}
	if (token == "pi") {
		token = to_string(pi);
		return Expression(token);
	}
	if (token == "e") {
		token = to_string(e);
		return Expression(token);
	}
	if (token == "(") {
		auto result = parse();
		if (parse_token() != ")") throw std::runtime_error("Expected ')'");
		return result;
	}
	//Если токен - цифра, то возвращаем выражение без аргументов
	if (std::isdigit(token[0]))
		return Expression(token);
	return Expression(token, parse_simple_expression());
}
//Функция для определения приоритета бинарных операций
int get_priority(const std::string& binary_op) {
	if (binary_op == "+") return 1;
	if (binary_op == "-") return 1;
	if (binary_op == "*") return 2;
	if (binary_op == "/") return 2;
	if (binary_op == "mod") return 2;
	if (binary_op == "**" || binary_op == "^") return 3;
	if (binary_op == ".") return 4;
	return 0;
}
Expression Parser::parse_binary_expression(int min_priority) {
	auto left_expr = parse_simple_expression();

	for (;;) {
		auto op = parse_token();
		auto priority = get_priority(op);
		//Если у выражения слева приоритет меньше, чем справа, возвращаем его
		if (priority <= min_priority) {
			input -= op.size();
			return left_expr;
		}
		//Парсим правую часть выражения
		auto right_expr = parse_binary_expression(priority);
		left_expr = Expression(op, left_expr, right_expr);
	}
}
Expression Parser::parse() {
	return parse_binary_expression(0);
}
double eval(const Expression& e) {
	switch (e.args.size()) {
	case 2: {
		auto a = eval(e.args[0]);
		auto b = eval(e.args[1]);
		if (e.token == "+") return a + b;
		if (e.token == "-") return a - b;
		if (e.token == "*") return a * b;
		if (e.token == "/") return a / b;
		if (e.token == ".") return a + 1 / b;
		if (e.token == "**" || e.token == "^") return pow(a, b);
		if (e.token == "mod") return (int)a % (int)b;
		throw std::runtime_error("Unknown binary operator");
	}
	case 1: {
		auto a = eval(e.args[0]);
		if (e.token == "+") return +a;
		if (e.token == "-") return -a;
		if (e.token == "abs") return abs(a);
		if (e.token == "sin") return sin(a);
		if (e.token == "cos") return cos(a);
		if (e.token == "tg") return tan(a);
		if (e.token == "ctg") return 1 / tan(a);
		if (e.token == "sec") return 1 / cos(a);
		if (e.token == "csec") return 1 / sin(a);
		if (e.token == "ln") return log(a);
		throw std::runtime_error("Unknown unary operator");
	}
	case 0:
		return strtod(e.token.c_str(), nullptr);
	}
	throw std::runtime_error("Unknown expression type");
}
void Euler(const char* input, double GX, double GY, double h, int n, double* arr, bool reduce) {
	cout << "\nn\tx\t" << "\ty";
	gX = GX;
	gY = GY;
	bool Red = reduce;
	int i = 0;
	int j = 1;
	int N = n * 2;
	double H = h / 2;
	double min = 0;
	cout << "\n" << i << "\t" << fixed << gX << "\t" << fixed << gY;
	if (reduce) {
		N = n;
		H = h;
	}
	for (int i = 0; i < N; i++)
	{
		Parser p(input);
		auto result = eval(p.parse());
		gX = gX + H;
		gY = gY + result * H;
		if (reduce) {
			arr[i] = gY;
		}
		else if (i != 0 && i % 2 == 1) {
			if (abs(arr[i - j] - gY) > min) {
				min = abs(arr[i - j] - gY);
				j++;
			}
		}
		else if (arr[0] - gY == 0)
			min = 0;
		cout << "\n" << i + 1 << "\t" << fixed << gX << "\t" << fixed << gY;
	}
	if (!reduce)
		cout << "\t\tAccuracy score = " << min;
}
void RungeKutt(const char* input, double GX, double GY, double h, int n, double* arr, bool reduce) {
	cout << "\nn\tx\t" << "\ty";
	double tempX = GX;
	double tempY = GY;
	bool Red = reduce;
	double min = 0;
	int i = 0;
	int j = 1;
	int N = n * 2;
	double H = h / 2;
	cout << "\n" << i << "\t" << gX << "\t" << tempY;
	if (reduce) {
		N = n;
		H = h;
	}
	for (int i = 0; i < N; i++)
	{
		double k[4];
		for (int j = 0; j < 4; j++) {
			Parser p(input);
			if (j == 0) {
				gX = tempX;
				gY = tempY;
				auto result = eval(p.parse());
				k[j] = H * result;
			}
			else if (j == 1) {
				gX = tempX;
				gY = tempY;
				gX = gX + H / 2;
				gY = gY + k[j - 1] / 2;
				auto result = eval(p.parse());
				k[j] = H * result;
			}
			else if (j == 2) {
				gX = tempX;
				gY = tempY;
				gX = gX + H / 2;
				gY = gY + k[j - 1] / 2;
				auto result = eval(p.parse());
				k[j] = H * result;
			}
			else if (j == 3) {
				gX = tempX;
				gY = tempY;
				gX = gX + H;
				gY = gY + k[j - 1];
				auto result = eval(p.parse());
				k[j] = H * result;
			}
		}
		tempX = gX;
		tempY = tempY + (k[0] + 2 * k[1] + 2 * k[2] + k[3]) / 6;
		if (reduce) {
			arr[i] = tempY;
		}
		else if (i != 0 && i % 2 == 1) {
			if (abs(arr[i - j] - tempY) / 15 > min) {
				min = abs(arr[i - j] - tempY) / 15;
				j++;
			}
		}
		else if (arr[0] - gY == 0)
			min = 0;
		cout << "\n" << i + 1 << "\t" << gX << "\t" << tempY;
	}
	if (!reduce)
		cout << "\t\tAccuracy score = " << min;
}
void NullArr(double* arr) {
	for (int i = 0; i < n * 2; i++)
		arr[i] = 0;
}
int errors;
int True;
void test(const char* input, double expected) {
	try {
		Parser p(input);
		auto result = eval(p.parse());
		if (result == expected) {
			++True;
			return;
		}
		std::cout << input << " = " << expected << " : error, got " << result << '\n';
	}
	catch (std::exception& e) {
		std::cout << input << " : exception: " << e.what() << '\n';
	}
	++errors;
}
void Test() {
	double gX = 1;
	double gY = 1;
	test("0", 0);
	test("1", 1);
	test("9", 9);
	test("10", 10);
	test("+1", 1);
	test("-1", -1);
	test("(1)", 1);
	test("(-1)", -1);
	test("abs(-1)", -1);
	test("1+20", 21);
	test("1 + 20", 21);
	test("1+20+300*cos(x)", 321);
	test("1+20+300+1234+x", 1555);
	test("-1+20", 19);
	test("--1+20", 21);
	test("---24+20", 4);
	test("(3+51)", 54);
	test("3*-7", -21);
	test("2*-3", -6);
	test("1++2", 3);
	test("abs2", 3);
	test("2+5*2**3", 42);
	test("1+2**3*10", 81);
	test("2**3+2*10", 28);
	test("5 * 4 + 3 * 2 + 1", 27);
	test("1+10*2+z", 21);
	test("10*2+1", 21);
	test("(1+20)*2", 42);
	test("2*(1+20)", 42);
	test("(1+2)*(3+4)", 21);
	test("2*3+4*5", 26);
	test("100+2*10+3", 123);
	test("2**3", 8);
	test("2**3*5+2", 42);
	test("5*2**3+2", 42);
	test("(1*-3)-(pi+y)", -1);
	test("1-17++124+e", 108);
	test("-*1+20", 19);
	test("*--1+20", 21);
	test("---2^3+20", 24);
	test("((3+51))", 54);
	test("3*-7", -21);
	test("2***2", 4);
	test("1*/2", 2);
}
int main() {
	Test();
	std::cout << "Done with " << errors << " errors,\t" << "True test - "<<True<<".\n";
	int what = 1;
	while (what != 0) {
		cout << "\nPress 1 to find out who is the Creator of the project\nPress 2 to read the manual" <<
			"\nPress 3 to see examples of ODE formulas\nPress 4 if you want to enter a new formula" <<
			"\nPress 0 if you want to end the program\n";
		(cin >> what).get();
		if (what == 1) {
			cout << "\nThe project was created by:\nStudent Kheylo D.G.\nGroups M3O-225Bk-21" << endl;
		}
		else if (what == 2) {
			cout << "\nThe program allows you to calculate ODE numerical methods:\n\t1)Euler method\n\t2)Runge-Kutta method." <<
				"\nThe error of the Euler method and the Runge-Kutta method depends on the integration step and\n on the properties of the differential equation being solved." <<
				"\nBoth methods are numerical methods for the approximate solution of differential equations.\nThe formula for the error of the Euler method is as follows:" <<
				"\n\tError = O(h^2),\n\twhere 'h' is the integration step.\nThe formula for the error of the fourth order Runge-Kutta method is as follows:\n\tError = O(h^4)," <<
				"\n\twhere h is the integration step\nIt is possible to use the following operations:\n'+'-addition,\n '-'-subtraction,/n'*'-multiplication,\n'/'-division," <<
				"\n'^' or '**' -degree,\n'.'-a fractional number,\n'abs'-absolute value of a number,\n'sin', 'cos', 'tg','ctg','sec','csec' and 'ln'. As well as the numbers 'e' and 'pi'\n";
		}
		else if (what == 3) {
			cout << "\nSolution of the Cauchy problem for a differential equation" <<
				"first order finite difference method Formula Examples:\n\t10*(x+2)*10+3*y\n\t3+2*x-y\n\ty--3*x\n\t-8 +2*x  -y\n\t4*cos(pi/4)";
		}
		else if (what == 4) {
			cout << "\nWrite the ODE formula\n" << "dy/dx =";
			string form;
			cin >> form;
			if (form == "") throw std::runtime_error("Invalid input");
			cout << "Enter initial value x: ";
			cin >> gX;
			gX1 = gX;
			cout << "Enter initial value y: ";
			cin >> gY;
			gY1 = gY;
			cout << "Enter step h: ";
			cin >> h;
			cout << "Enter number of iterations: ";
			cin >> n;
			if (n != 0) {
				double* arr = new double[n * 2];//Выделение памяти для определения точности
				NullArr(arr);
				cout << "\nEuler method, h = " << h;
				Euler(form.c_str(), gX, gY, h, n, arr, true);
				cout << endl;
				gX = gX1;
				gY = gY1;
				if (h != 0) {
					cout << "\nMethod Euler, h/2 = " << h / 2;
					Euler(form.c_str(), gX, gY, h, n, arr, false);
				}
				gX = gX1;
				gY = gY1;
				NullArr(arr);
				cout << "\nMethod Runge-Kutta, h = " << h;
				RungeKutt(form.c_str(), gX, gY, h, n, arr, true);
				cout << endl;
				gX = gX1;
				gY = gY1;
				if (h != 0) {
					cout << "\nMethod Runge-Kutta, h/2 = " << h / 2;
					Euler(form.c_str(), gX, gY, h, n, arr, false);
				}
				cout << endl;
				//cout << "\nIf you want to exit, press 0\n";
				//(cin >> what).get();
				delete[] arr;
			}
			else cout << "\nNumber of iterations = 0\n";
			form.erase();
			cin.ignore();
		}
	}
}
