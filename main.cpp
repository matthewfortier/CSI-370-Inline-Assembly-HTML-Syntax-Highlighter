#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <unordered_map>
#include <algorithm>
#include <windows.h>
using namespace std;

enum SyntaxColors {
	INSTRUCTIONS,
	REGISTERS,
	DIRECTIVES,
	JUMP,
	LABELS,
	CONSTANT,
	BACKGROUND,
	COMMENT,
	PLAIN
};

string returnSpan(string input, string color, string style) {
	return "<span style='color:" + color + ";" + style + "'>" + input + "</span>";
}

extern "C" int _stdcall checkColor(unordered_map<string, string> &keys, vector<string> &results, char buffer[], int count) {
	string temp = "";
	for (int i = 0; i < count; i++) {
		temp += buffer[i];
	}

	if (keys[temp] != "") {

		results.push_back(temp);
		results.push_back(keys[temp]);

	} /*else if (isdigit(temp[0])) {
		results.push_back(temp);
		results.push_back("#FFD269");
	}*/

	return 0;
}

int main() {

	ofstream fout;
	ifstream fin;

	string temp = "";
	int lineLength = 0;
	int found = 0;
	int next = 0;
	int colorCount = -1;
	char* tempAddress;
	
	DWORD count = 0;
	byte buffer[256] = "";

	string instruction;
	string color;
	string comment;
	string replaceString = "";
	unordered_map<string, string> keys;

	vector<string> results;
	vector<string> colors;

	fin.open("colors.txt");

	while (!fin.eof()) {
		getline(fin, color);
		color.erase(color.begin(), color.end() - 7);
		colors.push_back(color);
	}

	fin.close();

	fin.open("db.txt");

	while (!fin.eof()) {
		getline(fin, instruction);
		if (instruction[0] == '#') {
			colorCount++;
		}
		else {
			keys[instruction] = colors[colorCount];
		}
	}

	fin.close();

	fin.open("code.asm");
	fout.open("code.html");

	fout << "<style>body{padding: 0; margin: 0;}</style>" << endl;
	fout << "<pre style='padding: 15px; color: " + colors[PLAIN] + "; background: " + colors[BACKGROUND] + ";'>" << endl;
	fout << "<h1>Assembly Syntax Highlighting</h1>" << endl;
	fout << "<h2>Matthew Fortier</h2>" << endl;

	while (!fin.eof()) {

		getline(fin, temp);
		lineLength = temp.length();

		if (keys[temp] != "") {

			replaceString = returnSpan(temp, keys[temp], "font-weight: bold;");
			temp = replaceString;

		}
		else if (temp[temp.length() - 1] == ':') {

			replaceString = returnSpan(temp, colors[LABELS], "font-weight: bold;");
			temp = replaceString;

		}
		else {

			tempAddress = &temp[0];

			char tempChar;

			_asm {

				mov ecx, lineLength
				cmp ecx, 0
				je endLine

				mov esi, tempAddress

				mov count, 0

				parseLine:

					lodsb
					mov tempChar, al

					cmp al, 0x3B
					je endLine

					cmp al, 0x3A
					je foundLetter

					cmp al, 0x2E
					je foundLetter

					cmp al, 0x2D
					je foundLetter

					cmp al, 0x30
					jl notLetter

					cmp al, 0x57
					jle foundLetter

					cmp al, 0x41
					jl notLetter

					cmp al, 0x5A
					jle foundLetter

					cmp al, 0x61
					jl notLetter

					cmp al, 0x7A
					jg notLetter

					foundLetter :

						lea edi, buffer
						mov edx, count
						mov[edi + edx], al
						inc count

						jmp continueParse

					notLetter :

						mov eax, count
						cmp eax, count
						jnz continueParse

						push count
						lea eax, buffer
						push eax
						lea eax, results
						push eax
						lea eax, keys
						push eax
						call checkColor

						mov count, 0

					continueParse:

				loop parseLine

			endLine :

			comment:

			}

			found = temp.find(';');
			if (found >= 0) {
				comment = temp.substr(found);
				replaceString = returnSpan(comment, colors[COMMENT], "");
				temp.replace(found, temp.length(), replaceString);
			}

			found = temp.find('"');
			next = temp.find('"', found + 1);
			if (found >= 0) {
				comment = temp.substr(found, next);
				replaceString = returnSpan(comment, colors[CONSTANT], "");
				temp.replace(found, next, replaceString);
			}

			if (results.size() != 0) {
				for (int i = 0; i < results.size(); i += 2) {
					replaceString = returnSpan(results[i], results[i + 1], "font-weight: bold;");
					found = temp.find(results[i]);
					if (found > 0) {
						temp.replace(temp.find(results[i]), results[i].length(), replaceString);
					}
				}
			}
		}

		fout << temp << endl;

		//cout << temp << endl;
		results.clear();
	}

	fout << "</pre>" << endl;

	fin.close();
	fout.close();

	ShellExecute(NULL, "open", "code.html",
		NULL, NULL, SW_SHOWNORMAL);

	return 0;
}