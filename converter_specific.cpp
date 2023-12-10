// This file contains converter-specific code.
#include <iostream>
#include <string>
#include <filesystem> // only works in C++17 and later, older versions are not supported!
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

#include "functions.h"
#include "converter_specific.h"

int ffmpeg(const std::string& input_file, const std::string& output_file) {
    // TODO: Use library, NOT the system install.
    // Using system() this way is unsafe!
    int exit_code = execSystem(std::string("ffmpeg -i \"" + input_file + "\" \"" + output_file + "\""));
    return exit_code;
}

int pandoc(const std::string& input_file, const std::string& output_file) {
    // TODO: Use library, NOT the system install.
    // Using system() this way is unsafe!
    // at least properly escape everything..
    int exit_code = execSystem(std::string("pandoc \"" + input_file + "\" -o \"" + output_file + "\""));
    return exit_code;
}

int soffice(const std::string& input_file, const std::string& output_file) {
    // Convert to /tmp/<filename>.<extension>, then move
    // TODO: Use library, NOT the system install.
    // Using system() this way is unsafe!
    // at least properly escape everything..
    std::string output_file_ext = getExtension(output_file);
    std::string output_file_name = std::filesystem::path(output_file).filename();
    std::string tmp_file = "/tmp/" + output_file_name;
    int exit_code = execSystem(std::string("soffice --headless --convert-to " + output_file_ext + " " + input_file + " --outdir /tmp/"));
    // Move /tmp/<output_file_name> to <output_file>
    std::ifstream ifs(tmp_file, std::ios::in | std::ios::binary);
    std::ofstream ofs(output_file, std::ios::out | std::ios::binary);
    ofs << ifs.rdbuf();
    std::filesystem::remove(tmp_file);
    return exit_code;
}
