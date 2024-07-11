#include <iostream>
#include <string>
#include <vector>
#include <fstream>

//File: <header_size>{filesize#filename#...}

struct File {
    std::string name;
    int64_t size = 0;
    int64_t padding = 0; // #padding from the beginning (without header_size) to get current file
    File* prev_file = nullptr;

    File() = default;
    File(uint64_t filesize, const std::string& filename, File* prev) : size(filesize), name(filename), padding(0), prev_file(prev) {
        if (prev != nullptr) {
            this->padding = prev->size + prev->padding;
        }
    }
};

struct Archive {
    std::vector<File> files;
    std::string path_with_filename; // real_bin_file
    std::string path;
    uint64_t file_info_size = 0; // header_size + {} + filesize#filename
    
    void create(const std::vector<std::string>& path);
    void extract(const std::vector<std::string>& files);
    void extract();
    void append(const std::string& file);
    void deleteFile(const std::string& file);
    void concatenate(const std::string& archive1, const std::string& archive2);

    void read(const std::string& file);
    void newFile(const std::string& file);
};

int numberSize(uint64_t num);
