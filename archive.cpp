#include "archive.h"

#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <filesystem>

int numberSize(uint64_t num) {
    int cnt = 0;
    while (num > 0) {
        cnt++;
        num /= 10;
    }
    return cnt;
}

void Archive::read(const std::string& file) {
    path_with_filename = file;
    path = file.substr(0, file.rfind('\\') + 1);
    
    std::ifstream bin_file(file, std::ios::binary);
    if (!bin_file.is_open()) {
        std::cerr << "The file can't be open";
        exit(1);
    }
    
    // digit reading of header_size
    int64_t cur_file_info_size = 0;
    char symb;
    while (bin_file.get(symb) && symb != '{') {
        if (symb < '0' || symb > '9') {
            std::cerr << "Not a number in header";
            exit(1);
        }
        cur_file_info_size = cur_file_info_size * 10 + (symb - '0');
    }

    char* file_info = new char[cur_file_info_size + 1]; // info in {}
    file_info[cur_file_info_size] = '\0';
    bin_file.read(file_info, cur_file_info_size); // read(<to>, <#symbs>)

    char c;
    if (bin_file.get(c) && c != '}') {
        std::cerr << "Invalid header or file input";
        exit(1);
    }

    std::string cur_size;
    std::string cur_name;
    std::vector<std::string> file_info_parsed;
    
    int cur_ind = 0;
    while (cur_ind < cur_file_info_size) {
        cur_size = "";
        cur_name = "";
        while (cur_ind < cur_file_info_size && file_info[cur_ind] != '#') {
            cur_size += file_info[cur_ind];
            cur_ind++;
        }
        file_info_parsed.push_back(cur_size);
        cur_ind++;
        while (cur_ind < cur_file_info_size && file_info[cur_ind] != '#') {
            cur_name += file_info[cur_ind];
            cur_ind++;
        }
        cur_ind++;
        file_info_parsed.push_back(cur_name);
    }

    for (int i = 0; i < file_info_parsed.size(); i += 2) {
        cur_size = file_info_parsed[i];
        cur_name = file_info_parsed[i + 1];
        
        File* prev = nullptr;
        if (!files.empty()) {
            prev = &files.back();
        }

        if (cur_size != "") {
            files.emplace_back(std::stoll(cur_size), cur_name, prev);
        }
    }

    file_info_size = bin_file.tellg(); // current file position (how many symbs did we read)
    bin_file.close();
}

void Archive::newFile(const std::string& file) {
    path_with_filename = file;
    path = file.substr(0, file.rfind('\\') + 1);
    
    std::ofstream bin_file(file, std::ios::binary);
    bin_file.write("0{}", 3); // empty archive
    file_info_size = 1;
    
    bin_file.close();
}

void Archive::create(const std::vector<std::string>& new_files) {
    std::vector<std::ifstream> inputs(new_files.size());
    uint64_t header_size = 0;
    file_info_size = 0;
    files.resize(new_files.size());
    
    for (int i = 0; i < new_files.size(); i++) {
        inputs[i].open(new_files[i], std::ios::binary);

        if (!inputs[i].is_open()) {
            std::cerr << "The file can't be opened";
            exit(1);
        }

        files[i].name = new_files[i].substr(new_files[i].rfind('\\') + 1, new_files[i].size());
        files[i].size = 0;
        if (i == 0) {
            files[i].padding = 0;
        } else {
            files[i].padding = files[i - 1].padding + files[i - 1].size;
        }

        inputs[i].seekg(0, std::ios::end);
        files[i].size = inputs[i].tellg();
        inputs[i].seekg(0);

        header_size += files[i].name.size() + numberSize(files[i].size) + 2; // size#name#
    }

    // write archive info to archive
    std::ofstream out;
    out.open(path_with_filename, std::ios::binary);
    out << header_size;
    file_info_size += numberSize(header_size);
    out << '{';
    for (int i = 0; i < files.size(); i++) {
        out << files[i].size << '#' << files[i].name << '#';
        file_info_size += files[i].name.size() + numberSize(files[i].size) + 2;
    }
    out << '}';
    file_info_size += 2;

    // write files to archive
    char symb;
    for (auto& input : inputs) {
        while (input.get(symb)) {
            out << symb;
        }
        input.close();
    }

    out.close();
}

void Archive::extract(const std::vector<std::string>& to_extract) {
    std::ifstream bin_file(path_with_filename, std::ios::binary);
    
    for (int i = 0; i < to_extract.size(); i++) {
        for (int j = 0; j < files.size(); j++) {
            if (to_extract[i] == files[j].name) {
                std::ofstream out(path + files[j].name, std::ios::binary);
                
                bin_file.seekg(file_info_size + files[j].padding); // move to position in file
                char symb;
                for (size_t k = 0; k < files[j].size; k++) {
                    bin_file.get(symb);
                    out << symb;
                }

                out.close();
                break;
            }
        }
    }
}

void Archive::extract() {
    std::ifstream bin_file(path_with_filename, std::ios::binary);
    for (int i = 0; i < files.size(); i++) {
        std::ofstream out(path + files[i].name, std::ios::binary);

        bin_file.seekg(file_info_size + files[i].padding);
        char symb;
        for (size_t i = 0; i < files[i].size; i++) {
            bin_file.get(symb);
            out << symb;
        }

        out.close();
    }
}

void Archive::append(const std::string& file) {
    // recreate archive with new added file
    
    size_t header_size = 0;
    std::string filename = file.substr(file.rfind('\\') + 1);

    for (int i = 0; i < files.size(); i++) {
        header_size += files[i].name.size() + numberSize(files[i].size) + 2;
        if (files[i].name == filename) {
            std::cerr << "The file already exists";
            exit(1);
        }
    }

    std::ifstream bin_file(file, std::ios::binary);
    if (!bin_file.is_open()) {
        std::cerr << "The file can't be opened";
        exit(1);
    }

    bin_file.seekg(0, std::ios::end);
    uint64_t size = bin_file.tellg();
    bin_file.seekg(0);

    std::ifstream other_bin_files(path_with_filename);
    other_bin_files.seekg(file_info_size);

    files.emplace_back(size, filename, &files.back());
    header_size += filename.size() + numberSize(size) + 2;

    std::string tmp_file = path + ".tmp";
    std::ofstream out(tmp_file);
    
    out << header_size << '{';
    for (int i = 0; i < files.size(); i++) {
        out << files[i].size << '#' << files[i].name << '#';
    }
    out << '}';

    char symb;
    for (int i = 0; i < files.size() - 1; i++) {
        for (int j = 0; j < files[i].size; j++) {
            other_bin_files.get(symb);
            out << symb;
        }
    }

    for (int j = 0; j < files.back().size; j++) {
        bin_file.get(symb);
        out << symb;
    }

    bin_file.close();
    out.close();

    std::filesystem::remove(path_with_filename);
    std::filesystem::rename(tmp_file, path_with_filename);
}

void Archive::deleteFile(const std::string& file) {
    int ind = -1;
    uint64_t file_info_size_no_del = file_info_size;
    file_info_size = 0;
    for (int i = 0; i < files.size(); i++) {
        if (files[i].name == file) {
            ind = i;
        } else {
            file_info_size += files[i].name.size() + numberSize(files[i].size) + 2;
        }
    }
    if (ind == -1) {
        return;
    }
    files.erase(files.begin() + ind);

    std::string tmp_file = path + ".tmp";
    std::ofstream out(tmp_file);
    
    out << file_info_size << '{';
    for (int i = 0; i < files.size(); i++) {
        out << files[i].size << '#' << files[i].name << '#';
    }
    out << '}';
    
    std::ifstream bin_file(path_with_filename);
    char symb;
    for (int i = 0; i < files.size(); i++) {
        bin_file.seekg(files[i].padding + file_info_size_no_del);
        for (int j = 0; j < files[i].size; j++) {
            bin_file.get(symb);
            out << symb;
        }
    }

    bin_file.close();
    out.close();

    std::filesystem::remove(path_with_filename);
    std::filesystem::rename(tmp_file, path_with_filename);
}

void Archive::concatenate(const std::string& archive1, const std::string& archive2) {
    Archive arch1;
    Archive arch2;
    arch1.read(archive1);
    arch2.read(archive2);

    files.resize(0);
    for (int i = 0; i < arch1.files.size(); i++) {
        for (int j = 0; j < arch2.files.size(); j++) {
            if (arch1.files[i].name == arch2.files[j].name) {
                std::cerr << "Found 2 identical files, can't be merged";
                exit(1);
            }
        }
    }

    uint64_t header_size = 0;
    for (int i = 0; i < arch1.files.size(); i++) {
        File* prev = nullptr;
        if (!files.empty()) {
            prev = &files.back();
        }
        files.emplace_back(arch1.files[i].size, arch1.files[i].name, prev);
        header_size += arch1.files[i].name.size() + numberSize(arch1.files[i].size) + 2;
    }
    for (int i = 0; i < arch2.files.size(); i++) {
        File* prev = nullptr;
        if (!files.empty()) {
            prev = &files.back();
        }
        files.emplace_back(arch2.files[i].size, arch2.files[i].name, prev);
        header_size += arch2.files[i].name.size() + numberSize(arch2.files[i].size) + 2;
    }

    std::ifstream arch1_bin(archive1, std::ios::binary);
    std::ifstream arch2_bin(archive2, std::ios::binary);

    arch1_bin.seekg(arch1.file_info_size);
    arch2_bin.seekg(arch2.file_info_size);

    std::ofstream out(path_with_filename, std::ios::binary);
    out << header_size << '{';
    for (int i = 0; i < files.size(); i++) {
        out << files[i].size << '#' << files[i].name << '#';
    }
    out << '}';

    char symb;
    while (arch1_bin.get(symb)) {
        out << symb;
    }
    while (arch2_bin.get(symb)) {
        out << symb;
    }
}
