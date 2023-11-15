#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>

std::string readNthLine(const std::string& filePath, int n);
std::string getExtension(const std::string& fullPath);
std::string removeExtension(const std::string& filePath);
std::string getCurrentFilePath();
bool isWordPresent(const std::string& inputString, const std::string& word);
int execSystem(const std::string& command);
void show_error(const std::string& title , const std::string& text , const std::string& info , const bool is_error);

std::vector<std::string> getPossibleOutput(const std::string& input_file);
std::string getConverter(const std::string& input_file, const std::string& output_file);

void convert(const std::string& input_file, const std::string& output_file, const bool& useGUI);
void help();

#endif // FUNCTIONS_H
