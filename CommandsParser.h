#pragma once

#include <vector>
#include <string>

struct CommandsParser {
    std::vector<std::string> free_args;
    std::string archive;
    int word_size = 0;
    
    bool is_create = false;
    bool is_file = false;
    bool is_list = false;
    bool is_extract = false;
    bool is_append = false;
    bool is_delete = false;
    bool is_concatenate = false;
    bool is_word_size = false;
    
    char getCommand();

    CommandsParser(int argc, char* argv[]);
    CommandsParser() = default;
};

