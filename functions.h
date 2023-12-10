#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>

// Modify string representation of file path
std::string getExtension(const std::string& fullPath);
std::string removeExtension(const std::string& filePath);
std::string getFolder(const std::string& path);

std::string readNthLine(const std::string& filePath, int n);
std::string getCurrentFilePath();
int execSystem(const std::string& command);
void show_error(const std::string& title , const std::string& text , const std::string& info , const bool is_error, const bool use_gui);

std::vector<std::string> getPossibleOutput(const std::string& input_file);
std::string getConverter(const std::string& input_file, const std::string& output_file);

void help();

#endif // FUNCTIONS_H
