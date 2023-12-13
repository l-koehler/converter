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

int squashfs(const string& input_file, const string& output_file) {
	string input_file_ext = getExtension(input_file);
	string output_file_ext = getExtension(output_file);
	string tmp_dir = "/tmp/convert_squash/";
	vector<string> argument_list;
	int exit_status;
    // create temp dir (if it doesnt exist)
	std::filesystem::create_directory(tmp_dir);
	if (input_file_ext == "tar" || input_file_ext == "tgz") {
		// unpack to tpm_dir
		argument_list = {"tar", "-xvf", input_file, "-C", tmp_dir};
        cout << input_file << "--" << tmp_dir << endl;
		exit_status = execvpString(argument_list);
		if (exit_status != 0) { // if tar failed
			return exit_status;
		}
		// squash tmp_dir
		argument_list = {"mksquashfs", tmp_dir, output_file};
		exit_status = execvpString(argument_list);
	} else {
		// unsquash to tmp_dir
		argument_list = {"unsquashfs", "-d", tmp_dir, input_file};
		exit_status = execvpString(argument_list);
		if (exit_status != 0) { // if unsquash failed
			return exit_status;
		}
		// compress or pack tmp_dir
		if (output_file_ext == "tar") {
			argument_list = {"tar", "-cvf", output_file, tmp_dir};
		} else {
			argument_list = {"tar", "-czvf", output_file, tmp_dir};
		}
		exit_status = execvpString(argument_list);
		if (exit_status != 0) { // if tar failed
			return exit_status;
		}
	}
	// delete tmp_dir
	std::filesystem::remove_all(tmp_dir);
	return exit_status;
}