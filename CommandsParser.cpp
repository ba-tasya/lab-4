#include "CommandsParser.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

std::unordered_map<char, std::string> to_big_command = {{'c', "--create"}, {'f', "--file"}, {'l', "--list"}, {'x', "--extract"}, 
                                                        {'a', "--append"}, {'d', "--delete"}, {'A', "--concatenate"}, {'w', "--word-size"}};

CommandsParser::CommandsParser(int argc, char* argv[]) {
    int cnt_flags = 0;
    
    for (int i = 1; i < argc; i++) {
        std::string command = argv[i];
        if (command[0] != '-') {
            free_args.push_back(command);
            continue;
        }
        if (command.size() < 2) {
            std::cerr << "Invalid command\n";
            exit(1);
        } else if (command.size() == 2) {
            if (to_big_command.find(command[1]) != to_big_command.end()) {
                command = to_big_command[command[1]];
                if (command == "--file") {
                    i++;
                    if (i < argc) {
                        archive = argv[i];
                        is_file = true;
                        cnt_flags++;
                        continue;
                    } else {
                        std::cerr << "Invalid command\n";
                        exit(1);
                    }
                } else if (command == "--word-size") {
                    i++;
                    if (i < argc) {
                        word_size = atoi(argv[i]);
                        is_word_size = true;
                        cnt_flags++;
                        continue;
                    } else {
                        std::cerr << "Invalid command\n";
                        exit(1);
                    }
                }
            } else {
                std::cerr << "Invalid command\n";
                exit(1);
            }
        }
        if (command[1] != '-') {
            std::cerr << "Invalid command\n";
            exit(1);
        }
        
        command = command.erase(0, 2);
        
        if (command[0] == 'f') {
            if (command.size() > 4) {
                std::string act_command = command.substr(0, 4);
                if (act_command == "file") {
                    if (command[4] == '=') {
                        archive = command.substr(5);
                        is_file = true;
                        cnt_flags++;
                    } else {
                        std::cerr << "Invalid command\n";
                        exit(1);
                    }
                } else {
                    std::cerr << "Invalid command\n";
                    exit(1);
                }
            } else {
                std::cerr << "Invalid command\n";
                exit(1);
            }
        } else if (command[0] == 'w') {
            if (command.size() > 9) {
                std::string act_command = command.substr(0, 9);
                if (act_command == "word-size") {
                    if (command[9] == '=') {
                        std::string size = command.substr(10);
                        word_size = stoi(size);
                        is_word_size = true;
                        cnt_flags++;
                    } else {
                        std::cerr << "Invalid command\n";
                        exit(1);
                    }
                } else {
                    std::cerr << "Invalid command\n";
                    exit(1);
                }
            } else {
                std::cerr << "Invalid command\n";
                exit(1);
            }
        } else if (command == "create") {
            is_create = true;
            cnt_flags++;
        } else if (command == "list") {
            is_list = true;
            cnt_flags++;
        } else if (command == "extract") {
            is_extract = true;
            cnt_flags++;
        } else if (command == "append") {
            is_append = true;
            cnt_flags++;
        } else if (command == "delete") {
            is_delete = true;
            cnt_flags++;
        } else if (command == "concatenate") {
            is_concatenate = true;
            cnt_flags++;
        }
    }
    
    if (!is_file) {
        std::cerr << "No archive name\n";
        exit(1);
    }
    if (!is_word_size) {
        std::cerr << "No word size\n";
        exit(1);
    }
    if (cnt_flags > 3) {
        std::cerr << "Too many arguments\n";
        exit(1);
    }
    if (is_concatenate && free_args.size() < 2) {
        std::cerr << "There aren't 2 archives to concatenate\n";
        exit(1);
    }
}

char CommandsParser::getCommand() {
    if (is_create) {
        return 'c';
    }
    if (is_list) {
        return 'l';
    }
    if (is_extract) {
        return 'x';
    }
    if (is_append) {
        return 'a';
    }
    if (is_delete) {
        return 'd';
    }
    if (is_concatenate) {
        return 'A';
    }
}

