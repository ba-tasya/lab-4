#include "InOutBit.h"

struct Encoder {
    BitReader input;
    BitWriter output;
    
    std::string input_name;

    Encoder(const std::string& input, const std::string& output);

    void encode(int word_size);
};

struct Decoder {
    BitReader input;
    BitWriter output;
    
    std::string input_name;

    Decoder(const std::string& input, const std::string& output);

    void decode(int word_size);
};
