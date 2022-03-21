#pragma once

#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "bitstream.h"
#include "huffman_constants.h"
#include "huffmantree.h"

namespace Huffman {
    class Encoder {
    public:
        explicit Encoder(std::ostream& out);
        ~Encoder();

        void EncodeFile(std::istream& in, const std::string file_name);

    private:
        BitWriter writer_;
        bool first_file_;
        Huffman::Code one_more_file_code_;
        Huffman::Code archive_end_code_;

        std::vector<size_t> GetCharCount(std::istream& in, const std::string file_name) const;
        void WriteOutput(const std::vector<HuffmanTree::EncodedChar>& encoded_chars, const std::string& file_name);
    };

    class Decoder {
    public:
        explicit Decoder(std::istream& in);

        void DecodeFiles();

    private:
        BitReader reader_;

        bool WriteFile(HuffmanTree& tree);
        bool DecodeFile();
    };
}
