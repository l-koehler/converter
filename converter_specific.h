#ifndef CONVERTER_SPECIFIC_H
#define CONVERTER_SPECIFIC_H

#include <string>

using namespace std;

int userdefined(const string& cmd, const string& input_file,
           const string& output_file);
int ffmpeg(const string& input_file, const string& output_file);
int pandoc(const string& input_file, const string& output_file);
int soffice(const string& input_file, const string& output_file);

#endif