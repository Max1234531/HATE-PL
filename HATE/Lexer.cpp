#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>


using namespace std;

class Point {
private:
	map<string, Point*> next_;


public:

	bool is_final;

	Point() : is_final(true) {}

	Point* Next(string ch) {
		if (next_.find(ch) == next_.end()) {
			//cout << "NO WAY WITH: '" << ch << "'!\n";
			return nullptr;
		}
		if (is_final) return nullptr;
		return next_[ch];
	}
	void AddWay(Point* next, string ch) {
		is_final = false;
		next_[ch] = next;
	}

};
class Automat {
public:
	Automat() :Start(nullptr) {}
	Automat(Point* Start_) :Start(Start_) {}
	Point* Start;

};

static int line_counter = 1;
static const vector<string> letters = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
static const vector<string> numbers = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const vector<string> signs = { ".", ",", ";", "'", R"(")", ":" };
static const vector<string> operators = { "+", "-", "*", "/", "%", "^", "=", "|", "&", "<", ">", "!" };
static const set<string> reserved = { "str","dbl","endl", "void", "return", "cinout", "int", "double", "bool", "char", "if", "for", "while", "do", "foreach", "true", "false", "elif", "string", "or", "and", "new", "program", "in", "else", "break", "continue" };
static const set<string> operations = { "+", "++", "-", "--", "/", "*", "=", "%", "+=", "-=", "/=", "*=", "<", ">", "<=", ">=", "^", "<<", ">>", "!", "==" };
static const set<string> brackets = { "(", ")", "[", "]", "{", "}" };
static const set<string> signs_set = { ".", ",", ";", "'", R"(")", ":" };


static Automat Init() {
	Point* H = new Point();//START
	Point* L = new Point();//LETTERS
	Point* N = new Point();//NUMBERS
	Point* P = new Point();//PUNCTUATION
	Point* D = new Point();//OPERANDS
	Point* D2 = new Point();
	Point* B = new Point();
	Point* S = new Point();//END
	H->AddWay(B, "(");
	H->AddWay(B, "{");
	H->AddWay(B, "[");
	H->AddWay(B, "]");
	H->AddWay(B, "}");
	H->AddWay(B, ")");

#pragma region InitializeID
	for (string letter : letters) {
		H->AddWay(L, letter);
		L->AddWay(L, letter);
	}
	for (string number : numbers) {
		L->AddWay(L, number);
	}

	H->AddWay(L, "_");
	L->AddWay(L, "_");
	//L->AddWay(S, " ");
#pragma endregion InitializeID
#pragma region InitializeCONST
	for (string number : numbers) {
		H->AddWay(N, number);
		N->AddWay(N, number);
	}
	N->AddWay(N, ".");
	//N->AddWay(S, " ");

#pragma endregion InitializeCONST
#pragma region InitializePUNCTUATION
	for (string sign : signs) {
		H->AddWay(P, sign);
		//P->AddWay(S, " ");
	}
#pragma endregion InitializePUNCTUATION
#pragma region InitializeOPERATORS
	for (string operator_ : operators) {
		H->AddWay(D, operator_);
	}
	for (string operator_ : operators) {
		D->AddWay(D2, operator_);
	}

	//D->AddWay(S, " ");
	//D2->AddWay(S, " ");

#pragma endregion InitializeOPERATORS

	return Automat(H);
}

static bool isint(string s) {
	for (char c : s) {
		if (!isdigit(c)) return false;
	}
	return true;
}

static bool isdouble(string s) {
	bool was_dot = false;
	for (char c : s) {
		if (isdigit(c)) {
			continue;
		}
		else if (c == '.' and !was_dot) {
			was_dot = true;
			continue;
		}
		else {
			return false;
		}
	}
	if (was_dot == false) {
		return false;
	}
	return true;
}

static bool isword(string s) {
	for (int i = 0; i < s.size(); i++) {
		char c = s[i];
		if (i == 0 and !isalpha(c) and c != '_') {
			return false;
		}

		if (!isalpha(c) and c != '_' and !isdigit(c)) {
			return false;
		}
	}
	return true;
}

static string GetType(string lex) {

	if (lex == "") return "END";
	if (find(lex.begin(), lex.end(), '"') != lex.end() || lex == "'\n'") return "STRING CONST";
	if (isword(lex)) {
		if (reserved.find(lex) == reserved.end()) {
			return "ID";
		}
		else {
			for (char& c : lex) { c = toupper(c); }
			return lex;
		}
	}
	else if (operations.find(lex) != operations.end()) {
		if (lex == "+" || lex == "-") return "SIMPLE SIGN";
		else if (lex == "++" || lex == "--") return "INCREMENT";
		else if (lex == "/" || lex == "%") return "COMPLEX SIGN";
		else if (lex == ">" || lex == "<" || lex == ">=" || lex == "<=" || lex == "!=" || lex == "==") return "COMPARE OPERATOR";
		else {
			for (char& c : lex) { c = toupper(c); }
			return lex;
		}
	}
	else if (isint(lex)) {
		return "CONST";
	}
	else if (isdouble(lex)) {
		return "DCONST";
	}
	else if (signs_set.find(lex) != signs_set.end() || brackets.find(lex) != brackets.end()) {
		for (char& c : lex) { c = toupper(c); }
		return lex;
	}
	else return "UNKNOWN";

}//////-=

static Point* curr;

static const Automat G = Init();


//ÔÓÍÊÖÈß ÂÛÄÀÞÙÀß ËÅÊÑÅÌÛ 
static string NextLex(string& program) {
	string buff = "", word;
	bool skip_mode = 0;
	bool com_mode = 0;

	buff = "", word;
	curr = G.Start;
	skip_mode = 0;
	com_mode = 0;

	for (int i = 0; i < program.size(); ++i) {

		buff = program[i];
		if (buff == "#") {
			com_mode = !com_mode;
			continue;
		}
		if (buff == "\n" && !skip_mode) line_counter++;
		if (com_mode) {
			continue;
		}
		if (buff == R"(")" and skip_mode == false) {
			if (word != "") {
				program = program.substr(i, program.size() - i);
				cout << '"' << word << '"' << " - Type: " << GetType(word) << endl;
				return word;
			}
			word += buff;
			skip_mode = !skip_mode;
			continue;
		}

		else if (buff == R"(")" and skip_mode == true) {
			word += buff;
			skip_mode = !skip_mode;
			program = program.substr(i + 1, program.size() - i);
			cout << '"' << word << '"' << " - Type: " << GetType(word) << endl;
			return word;
			continue;
		}

		if (skip_mode) {
			word += buff;
			continue;
		}


		if (curr->Next(buff) != nullptr) {
			curr = curr->Next(buff);
			word += buff;
		}
		else {
			if (word == "") {
				if (buff == " " || buff == "\t" || buff == "\n") {
					continue;
				}
			}
			program = program.substr(i, program.size() - i);
			cout << '"' << word << '"' << " - Type: " << GetType(word) << endl;
			return word;
		}
	}



}


