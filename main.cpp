#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <unordered_map>
#include <windows.h>
using namespace std;

string returnSpan(string input) {
	return "<span>" + input + "</span>";
}

string returnSpan(string input, string color, string style) {
	return "<span style='color:" + color + ";" + style + "'>" + input + "</span>";
}

string returnSpanCat(string str, string sub, int length, int location) {
	return str.replace(location, length, sub);
}

extern "C" void _stdcall debugPrint(char str) {
	cout << str;
}

extern "C" void _stdcall printString(char buffer[], int count) {
	for (int i = 0; i < count; i++) {
		cout << buffer[i];
	}
	cout << endl;
}

extern "C" int _stdcall checkColor(unordered_map<string, string> &keys, char* tempAddress, char buffer[], int count) {
	string temp = "";
	for (int i = 0; i < count; i++) {
		temp += buffer[i];
	}

	if (keys[temp] != "") {
		string str;
		str.assign(tempAddress);
		cout << str.find(temp) << endl;
		return str.find(temp);
	}

	return -1;
}

vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

int main() {

	ofstream fout;
	ifstream fin;

	string temp = "";
	string substr = "";
	char format[] = "%s";
	int lineLength = 0;
	char* tempAddress;
	byte found = 0;

	int start = 0;
	int end = 0;
	DWORD count = 0;
	byte buffer[256] = "";

	fin.open("instructions.txt");

	unordered_map<string, string> keys;
	string instruction;
	while (!fin.eof()) {
		getline(fin, instruction);
		keys[instruction] = "blue";
	}

	fin.close();

	fin.open("code.asm");
	fout.open("code.html");

	//fout << "<pre style='color: #000000; background: #ffffff;'>" << endl;
	//fout << "<h1>Assembly Syntax Highlighting</h1>" << endl;

	while (!fin.eof()) {

		getline(fin, temp);
		lineLength = temp.length();

		tempAddress = &temp[0];

		char tempChar;

		_asm {

			mov ecx, lineLength
			cmp ecx, 0
			je endLine

			mov esi, tempAddress

			parseLine:
				
			lodsb
				mov tempChar, al

				cmp al, '\0'
				je endLine

				cmp al, '\n'
				je endLine

				cmp al, ','
				je endLine

				; cmp al, ';'
				; je comment

				cmp al, 32
				je notLetter

				cmp al, '\t'
				je notLetter
				
				lea edi, buffer
				mov edx, count
				mov [edi + edx], al
				inc count

				jmp continueParse

				notLetter:
					
					mov eax, count
					cmp eax, count
					jnz continueParse

					push count
					lea eax, buffer
					push eax
					push tempAddress
					lea eax, keys
					push eax
					call checkColor
				
					mov count, 0
				
				continueParse:

				loop parseLine

			endLine:
			
			comment:

		}

		cout << endl;
	}

	//fout << "</pre>" << endl;

	fin.close();
	fout.close();

	//ShellExecute(NULL, "open", "code.html",
	//	NULL, NULL, SW_SHOWNORMAL);

	system("pause");

	return 0;
}