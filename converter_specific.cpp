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

using namespace std;

int userdefined(const string& command,
           const string& input_file, const string& output_file) {
    // command is the command in the custom typefile
    vector<string> argument_list {command, input_file, output_file};
    return execvpString(argument_list);
}

int ffmpeg(const string& input_file, const string& output_file) {
    vector<string> argument_list {"ffmpeg", "-i", input_file, output_file};
    return execvpString(argument_list); // execvpString returns the exit code
}

int pandoc(const string& input_file, const string& output_file) {
    vector<string> argument_list {"pandoc", "-s", input_file,
        "-o", output_file};
    return execvpString(argument_list);
}

int soffice(const string& input_file, const string& output_file) {
    // Convert to /tmp/<filename>.<extension>, then move
    string output_file_ext = getExtension(output_file);
    string output_file_name = filesystem::path(output_file).filename();
    string tmp_file = "/tmp/" + output_file_name;
    const char* const argument_list[] = {
        "soffice", "--headless",
        "--convert-to", output_file_ext.c_str(),
        input_file.c_str(),
        "--outdir", "/tmp/",
        nullptr
    };
    int status;
    int exit_status;
    cout << "Forking..." << endl;
    int pid = fork();
    if (!pid) {
        execvp(argument_list[0], const_cast<char*const*>(argument_list));
    }
    waitpid(pid, &status, 0);
    cout << "Converter thread done." << endl;
    if (WIFEXITED(status)) {
        exit_status = WEXITSTATUS(status);
    } else {
        exit_status = -1;
    }
    // Move /tmp/<output_file_name> to <output_file>
    ifstream ifs(tmp_file, ios::in | ios::binary);
    ofstream ofs(output_file, ios::out | ios::binary);
    ofs << ifs.rdbuf();
    filesystem::remove(tmp_file);
    return exit_status;
}
