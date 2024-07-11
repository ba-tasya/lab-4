#include "Hamming.h"

#include <filesystem>

Encoder::Encoder(const std::string &input, const std::string &output) {
    this->input = BitReader{input};
    this->output = BitWriter{output};
    this->input_name = input;
}

void Encoder::encode(int word_size) {
    word_size *= 8;
    while (!input.isEnded()) {
        int hamm_bits = static_cast<int>(trunc(log2(word_size)) + 1);
        int cur_2_deg = 1;
        int next_word_bit = 0;

        std::vector<bool> word = input.getCntNext(word_size);
        std::vector<bool> number(word_size + hamm_bits);
        std::vector<int> pref_sum(word_size + hamm_bits, 0);

        for (int i = 0; i < word_size + hamm_bits; i++) {
            pref_sum[i] = ((i == 0) ? 0 : pref_sum[i - 1]);
            if (i == cur_2_deg - 1) {
                cur_2_deg *= 2;
            } else {
                number[i] = word[next_word_bit];
                next_word_bit++;
                pref_sum[i] += number[i];
            }
        }

        int sum;
        for (int i = 1; i <= word_size + hamm_bits; i *= 2) {
            sum = 0;
            for (int j = i; j <= word_size + hamm_bits; j += i * 2) {
                sum += pref_sum[std::min(j + i - 1, word_size + hamm_bits - 1)] - pref_sum[j - 2];
            }
            number[i - 1] = sum % 2;
        }

        output.writeBits(number);
    }

    input.close();
    output.close();
    //std::filesystem::remove(input_name);
}

Decoder::Decoder(const std::string &input, const std::string &output) {
    this->input = BitReader(input);
    this->output = BitWriter(output);
    this->input_name = input;
}

void Decoder::decode(int word_size) {
    word_size *= 8;
    while (!input.isEnded()) {
        int hamm_bits = static_cast<int>(trunc(log2(word_size)) + 1);
        int cur_2_deg = 1;
        int next_word_bit = 0;

        std::vector<bool> hammed = input.getCntNext(word_size + hamm_bits);
        std::vector<bool> number(word_size);
        std::vector<int> pref_sum(word_size + hamm_bits, 0);

        for (int i = 0; i < word_size + hamm_bits; i++) {
            pref_sum[i] = ((i == 0) ? 0 : pref_sum[i - 1]);

            if (i == cur_2_deg - 1) {
                cur_2_deg *= 2;
            } else {
                pref_sum[i] += (!hammed[i] ? 0 : 1);
            }
        }

        int wrong_bit = 0;
        int sum;
        for (int i = 1; i <= word_size + hamm_bits; i *= 2) {
            sum = 0;
            for (int j = i; j <= word_size + hamm_bits; j += i * 2) {
                sum += pref_sum[std::min(j + i - 1, word_size + hamm_bits - 1)] - pref_sum[j - 2];
            }

            if (hammed[i - 1] != sum % 2) {
                wrong_bit += i - 1;
                hammed[i - 1] = !hammed[i - 1];
            }
        }
        if (wrong_bit != 0) {
            if (wrong_bit > word_size + hamm_bits || ((wrong_bit - 1) & wrong_bit) == 0) {
                std::cout << "More than 1 mistake, can't be corrected" << std::endl;
            } else {
                std::cout << "Bit " << wrong_bit << " was wrong. It was corrected" << std::endl;
                hammed[wrong_bit - 1] = !hammed[wrong_bit - 1];
            }
        }

        for (int i = 0; i < word_size + hamm_bits; i++) {
            if ((i & (i + 1)) != 0) { // i is not 2_deg
                number[next_word_bit] = hammed[i];
                next_word_bit++;
            }
        }

        output.writeBits(number);
    }

    input.close();
    output.close();
}
