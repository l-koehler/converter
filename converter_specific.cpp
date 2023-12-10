// This file contains converter-specific code.
#include <iostream>
#include <string>
#include <filesystem> // only works in C++17 and later, older versions are not supported!
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "functions.h"
#include "converter_specific.h"

int ffmpeg(const std::string& input_file, const std::string& output_file) {
    const char* const argument_list[] = {
        "ffmpeg",
        "-i", input_file.c_str(),
        output_file.c_str(),
        nullptr
    };
    int status;
    std::cout << "Forking..." << std::endl;
    int pid = fork();
    if (!pid) {
        execvp(argument_list[0], const_cast<char*const*>(argument_list));
    }
    waitpid(pid, &status, 0);
    std::cout << "Converter thread done." << std::endl;
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        return exit_status;
    }
    return -1;
}

int pandoc(const std::string& input_file, const std::string& output_file) {
    const char* const argument_list[] = {
        "pandoc",
        input_file.c_str(),
        "-o", output_file.c_str(),
        nullptr
    };
    int status;
    std::cout << "Forking..." << std::endl;
    int pid = fork();
    if (!pid) {
        execvp(argument_list[0], const_cast<char*const*>(argument_list));
    }
    waitpid(pid, &status, 0);
    std::cout << "Converter thread done." << std::endl;
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        return exit_status;
    }
    return -1;
}

int soffice(const std::string& input_file, const std::string& output_file) {
    // Convert to /tmp/<filename>.<extension>, then move
    std::string output_file_ext = getExtension(output_file);
    std::string output_file_name = std::filesystem::path(output_file).filename();
    std::string tmp_file = "/tmp/" + output_file_name;
    const char* const argument_list[] = {
        "soffice", "--headless",
        "--convert-to", output_file_ext.c_str(),
        input_file.c_str(),
        "--outdir", "/tmp/",
        nullptr
    };
    int status;
    int exit_status;
    std::cout << "Forking..." << std::endl;
    int pid = fork();
    if (!pid) {
        execvp(argument_list[0], const_cast<char*const*>(argument_list));
    }
    waitpid(pid, &status, 0);
    std::cout << "Converter thread done." << std::endl;
    if (WIFEXITED(status)) {
        exit_status = WEXITSTATUS(status);
    } else {
        exit_status = -1;
    }
    // Move /tmp/<output_file_name> to <output_file>
    std::ifstream ifs(tmp_file, std::ios::in | std::ios::binary);
    std::ofstream ofs(output_file, std::ios::out | std::ios::binary);
    ofs << ifs.rdbuf();
    std::filesystem::remove(tmp_file);
    return exit_status;
}
