#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>

using namespace std;

// Modify string representation of file path
string getExtension(const string& fullPath);
string removeExtension(const string& filePath);
string getFolder(const string& path);

string readNthLine(const string& filePath, int n);
string getCurrentFilePath();
int execSystem(const string& command);
int execvpString(const vector<string>& cmdline);
void show_error(const string& title , const string& text , const string& info , const bool is_error, const bool use_gui);

vector<string> getPossibleOutput(const string& input_file);
string getConverter(const string& input_file, const string& output_file);

void help();

#endif // FUNCTIONS_H
