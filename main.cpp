/*
Matthew Fortier
CSI-370-01 Computer Architecture
5/1/2017
Inline Assembly Final Project
*/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
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

// Returns HTML formatted span string with given color, style, and text
string returnSpan(string input, string color, string style) {
	return "<span style='color:" + color + ";" + style + "'>" + input + "</span>";
}

// Returns 1 if the specific keywords color exits in the hashmap, 0 if it does not
// This fucntion checks to see if the given word is a MASM instruction, directive, jump, or register
// If so, the function will add the keyword and color to a passed in vector for later use
// Also, if the word is not a keyword and is insteas a numeric constant, add that constant with its color to the vector
int _stdcall checkColor(unordered_map<string, string> &keys, vector<string> &results, vector<string> &colors, char buffer[], int count) {

	string temp = "";						// Converting char array to usable string
	for (int i = 0; i < count; i++) {
		temp += buffer[i];
	}

	if (temp.length() == 0) {				// If temp is length 0 skip it

		return 0;

	} else if (keys[temp] != "") {			// Checks hashmap for the given key

		results.push_back(temp);
		results.push_back(keys[temp]);
		return 1;

	} else if (isdigit(temp[1])) {			// Temporary fix for duplicate numbers in line 

		results.push_back(temp);
		results.push_back(colors[CONSTANT]);
		return 1;

	}

	return 0;

}

int main() {

	ofstream fout;
	ifstream fin;

	int lineLength = 0;
	int found = 0;
	int next = 0;
	int colorCount = -1;
	char* tempAddress;
	
	DWORD count = 0;
	byte buffer[256] = "";

	string instruction;
	string style;
	string color;
	string comment;
	string replaceString = "";
	string temp = "";

	string colorFile = "";
	string codeFile = "";
	string htmlFile = "";

	// Initializing hashmap to hold keyword and color strings
	unordered_map<string, string> keys;

	// Initializing vectors
	vector<string> results;
	vector<string> colors;

	// User I/O for selecting which theme and target file for highlighting
	cout << "Enter MASM Assembly File Filename (code.asm): " << endl;
	cout << "> ";
	getline(cin, codeFile);

	cout << endl << "Enter Syntax Theme Choice (no file extension): " << endl;
	cout << "Theme Options: (facebook or colors):" << endl;
	cout << "> ";
	getline(cin, colorFile);

	cout << endl;

	// Open and confirm success
	fin.open(colorFile + ".txt");
	if (fin.fail()) {

		cout << "Syntax Theme Reuqested Does Not Exist -- Exiting Program" << endl;
		system("pause");
		return 0;

	}

	while (!fin.eof()) {

		getline(fin, color);
		color.erase(color.begin(), color.end() - 7); // Get only the final 7 characters of color line which is the hex value
		colors.push_back(color);					 // Push each color on to the colors vector

	}

	fin.close();

	// Open and test database file
	fin.open("db.txt");
	if (fin.fail()) {

		cout << "Failed To Open Database File -- Exiting Program" << endl;
		system("pause");
		return 0;

	}

	// Add all instructions, directives, registers, and jumps to hashmap with repective color
	while (!fin.eof()) {

		getline(fin, instruction);
		if (instruction[0] == '#') {

			colorCount++;							// Increment color count to move to next color when database section changes

		}
		else {

			keys[instruction] = colors[colorCount]; // Create key in hashmap with value as the color

		}

	}

	fin.close();

	// Open HTML Output file and read in and print out styles
	htmlFile = codeFile + ".html";
	fin.open("styles.css");
	fout.open(htmlFile);

	while (!fin.eof()) {

		getline(fin, style);
		fout << style << endl;

	}

	// Print initial and important HTML tags to file first
	fout << "<pre style='padding: 15px; color: " + colors[PLAIN] + "; background: " + colors[BACKGROUND] + ";'>" << endl;
	fout << "<h1>MASM Assembly Syntax Highlighting</h1>" << endl;
	fout << "<h2>Matthew Fortier</h2>" << endl;
	fout << "<h3>May 1, 2017</h3>" << endl;

	fin.close();

	// Open and test codefile
	fin.open(codeFile);
	if (fin.fail()) {

		cout << "MASM Assembly File Reuqested Does Not Exist -- Exiting Program" << endl;
		system("pause");
		return 0;

	}

	// Begin reading each line of code file
	while (!fin.eof()) {

		getline(fin, temp);
		lineLength = temp.length();

		// Test to see if entire string is a keyword
		if (keys[temp] != "") {

			replaceString = returnSpan(temp, keys[temp], "font-weight: bold;");
			temp = replaceString;

		}
		// Test to see if final character is a color to make it a label
		else if (temp[temp.length() - 1] == ':') {

			replaceString = returnSpan(temp, colors[LABELS], "font-weight: bold;");
			temp = replaceString;

		}
		else {

			// Parse entire line for each word
			// Get the actual memory location of the line string
			tempAddress = &temp[0];

			char tempChar;

			_asm {

				mov esi, tempAddress;	Move the string to esi so the chracters can be accessed one at a time

				mov count, 0;			Initialize the count to 0	

				parseLine:

					lodsb;				Load the current character into al based on implicit count
					mov tempChar, al

					cmp al, 0x3B;		Determine if character is considered a letter or not 
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

					foundLetter:

						lea edi, buffer;		If a valid letter was found, add it to the buffer and increment the count
						mov edx, count
						mov[edi + edx], al
						inc count

						jmp continueParse;		Continue parsing the line

					notLetter:

						push count;				If the word is a valid length, check to see if it should be styled
						lea eax, buffer
						push eax
						lea eax, colors
						push eax
						lea eax, results
						push eax
						lea eax, keys
						push eax
						call checkColor

						mov count, 0;			Reset the count after the word was checked

					continueParse:

				loop parseLine;					Continue to loop until the end of the string

			endLine :

			}

			// Check line for comment sections
			found = temp.find(';');
			if (found >= 0) {

				// If comment section was found, the rest of the string from the found location is included
				comment = temp.substr(found);
				replaceString = returnSpan(comment, colors[COMMENT], "");
				// Replace the section in the orignal string with styled section
				temp.replace(found, temp.length(), replaceString);

			}

			// Check line for call instructions to style following function name
			found = temp.find("call");
			if (found >= 0) {

				comment = temp.substr(found);
				replaceString = returnSpan(comment, colors[LABELS], "");
				temp.replace(found, temp.length(), replaceString);

			}

			// Check line for PROTO directtive and style the previous function declarations
			found = temp.find("PROTO");
			if (found >= 0) {

				comment = temp.substr(0, found);
				replaceString = returnSpan(comment, colors[LABELS], "");
				temp.replace(0, found, replaceString);

			}

			// Check line for constant string sections
			found = temp.find('"');
			next = temp.find('"', found + 1);
			if (found >= 0) {

				comment = temp.substr(found, next);
				replaceString = returnSpan(comment, colors[CONSTANT], "");
				temp.replace(found, next, replaceString);

			}

			// After checking for specific rules, replace keywords with styled versions
			if (results.size() != 0) {

				// Iterate through results vector every 2 items for each keyword
				for (int i = 0; i < results.size(); i += 2) {

					// Generate the replace string with the current item being the keyword and the next being color
					replaceString = returnSpan(results[i], results[i + 1], "font-weight: bold;");
					found = temp.find(results[i], 0);

					// Only if the keyword is found once more should it be replace in the final string
					if (found > 0) {

						temp.replace(temp.find(results[i]), results[i].length(), replaceString);

					}

				}

			}

		}

		// Print each line to the output file
		fout << temp << endl;

		// Clear the results vector for the next line
		results.clear();
	}

	// Close the pre tag that was added towards the beginning of the program
	fout << "</pre>" << endl;

	// Close both files
	fin.close();
	fout.close();

	// Open the syntax highlighted HTML file
	ShellExecute(NULL, "open", htmlFile.c_str(),
		NULL, NULL, SW_SHOWNORMAL);

	return 0;
}