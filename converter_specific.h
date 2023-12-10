#ifndef CONVERTER_SPECIFIC_H
#define CONVERTER_SPECIFIC_H

#include <string>

int ffmpeg(const std::string& input_file, const std::string& output_file);
int pandoc(const std::string& input_file, const std::string& output_file);
int soffice(const std::string& input_file, const std::string& output_file);

#endif