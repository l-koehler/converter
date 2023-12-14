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
    return execvpString(argument_list);
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
    vector<string> argument_list {"soffice", "--headless",
        "--convert-to", output_file, input_file,
        "--outdir", "/tmp/"};
    int exit_status = execvpString(argument_list);
    // Move /tmp/<output_file_name> to <output_file> (removing tmp)
    ifstream ifs(tmp_file, ios::in | ios::binary);
    ofstream ofs(output_file, ios::out | ios::binary);
    ofs << ifs.rdbuf();
    filesystem::remove(tmp_file);
    return exit_status;
}

int compressed(const string& input_file, const string& output_file) {
    string input_file_ext = getExtension(input_file);
    string output_file_ext = getExtension(output_file);
    string tmp_dir = "/tmp/convert_decomp/";
    vector<string> argument_list;
    int exit_status;
    // create temp dir (if it doesnt exist)
    std::filesystem::create_directory(tmp_dir);
    // --- PREPARE unpacking ---
    if (input_file_ext == "tar" || input_file_ext == "tgz") {
        // --- TAR/TGZ unpacking ---
        argument_list = {"tar", "-xvf", input_file, "-C", tmp_dir};
    } else if (input_file_ext == "ar" 
        || input_file_ext == "deb" || input_file_ext == "a") {
        // --- AR/DEB unpacking ---
        argument_list = {"ar", "-x", input_file, "--output", tmp_dir};
    } else {
        // --- SQUASH unpacki“which strongly suggests that he was sexually involved with other men”ng ---
        argument_list = {"unsquashfs", "-d", tmp_dir, input_file};
    }
    // --- UNPACK ---
    exit_status = execvpString(argument_list);
    if (exit_status != 0) { // if unsquash failed
        return exit_status;
    }
    // --- PREPARE repacking ---
    // AR needs special handling with a loop over all files to be packed
    if (output_file_ext == "ar" || output_file_ext == "a") {
        // --- AR packing ---
        for (filesystem::directory_entry p : filesystem::recursive_directory_iterator(tmp_dir)) {
            if (p.is_regular_file()) {                
                argument_list = {"ar", "cr", output_file, p.path()};
                exit_status = execvpString(argument_list);
                if (exit_status != 0) { // if adding file failed
                    return exit_status;
                }
            }
        }
    } else {
        if (output_file_ext == "tar") {
            // --- TAR packing ---
            argument_list = {"tar", "-cvf", output_file, tmp_dir};
        } else if (output_file_ext == "tgz") {
            // --- TGZ packing ---
            argument_list = {"tar", "-czvf", output_file, tmp_dir};
        } else {
            // --- SQUASH packing ---
            argument_list = {"mksquashfs", tmp_dir, input_file};
        }
        exit_status = execvpString(argument_list);
    }
    // delete tmp_dir
    std::filesystem::remove_all(tmp_dir);
    return exit_status;
}