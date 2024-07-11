#include <fstream>
#include <vector>
#include <bitset>
#include <cmath>
#include <queue>
#include <iostream>

// class to bit reading

struct BitReader {
public:
    std::ifstream input;
    std::queue<bool> extra_bits;
    
    bool isEnded();
    bool getNext();
    std::vector<bool> getCntNext(int cnt);
    
    explicit BitReader(const std::string& file);
    BitReader() = default;
    
    void close();
};

struct BitWriter {
public:
    std::ofstream output;
    std::queue<bool> extra_bits;

    void writeBits(const std::vector<bool>& bits);
    void isExtraBits();
    
    explicit BitWriter(const std::string& output_file);
    BitWriter() = default;
    
    void close();
};
