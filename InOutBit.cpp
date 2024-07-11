#include "InOutBit.h"

// Reading
bool BitReader::isEnded() {
    return input.eof() && extra_bits.empty();
}

bool BitReader::getNext() {
    if (isEnded()) {
        return 0;
    }
    
    bool next_bit;
    
    if (extra_bits.empty()) {
        char symb;
        input.get(symb); // to read all symbols, including whitespaces and ...

        for (int i = 7; i >= 0; i--) {
            extra_bits.push(symb & (1 << i));
        }
    }

    next_bit = extra_bits.front();
    extra_bits.pop();

    return next_bit;
}

std::vector<bool> BitReader::getCntNext(int cnt) {
    std::vector<bool> next_cnt_bits(cnt, 0);
    
    for (int i = 0; i < cnt; i++) {
        next_cnt_bits[i] = getNext();
    }

    return next_cnt_bits;
}

void BitReader::close() {
    input.close();
}

BitReader::BitReader(const std::string& file) {
    input.open(file, std::ios::binary); // open in a binary view

    if (!input.is_open()) {
        std::cerr << "File " << file << " doesn't exist\n";
        exit(1);
    }
}

// Writing
void BitWriter::writeBits(const std::vector<bool>& bits) {
    for (int i = 0; i < bits.size(); i++) {
        extra_bits.push(bits[i]);
        
        if (extra_bits.size() == 8) {
            uint8_t symb; // 8 bit
        
            for (int j = 0; j < 8; j++) {
                symb = (symb << 1) + extra_bits.front();
                extra_bits.pop();
            }
    
            output << symb;
        }
    }
}

void BitWriter::close() {
    isExtraBits();
    output.close();
}

BitWriter::BitWriter(const std::string& file) {
    output.open(file, std::ios::binary);

    if (!output.is_open()) {
        std::cerr << "File " << file << " doesn't exist\n";
        exit(1);
    }
}

// 1 time (when end of file) add 1-7 '0' to :8
void BitWriter::isExtraBits() {
    if (!extra_bits.empty()) {
        char symb;
        int cnt = 0;
        
        while (!extra_bits.empty()) {
            symb = (symb << 1) + extra_bits.front();
            extra_bits.pop();
            cnt++;
        }
        while (cnt < 8) {
            symb = (symb << 1);
            cnt++;
        }

        output << symb;
    }
}
