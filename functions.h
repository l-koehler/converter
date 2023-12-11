#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>

using namespace std;

// Modify string representation of file path
string getExtension(const string& fullPath);
string removeExtension(const string& filePath);
string getFolder(const string& path);
// Other file stuff
string readNthLine(const string& filePath, int n);
string getCurrentFilePath();

// Execute argv
int execvpString(const vector<string>& cmdline);
// Show an error using either the GUI or stderr
void show_error(const string& title, const string& text, const string& info,
                const bool is_error, const bool use_gui);

// Get all possible formats a given extension can convert to
vector<string> getPossibleOutput(const string& input_file);
// Get the converter name for a input-output pair
string getConverter(const string& input_file, const string& output_file);

// write help to stdout
void help();

#endif // FUNCTIONS_H
