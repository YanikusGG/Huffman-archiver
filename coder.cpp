#include "coder.h"

#include <optional>

namespace Huffman {
    Encoder::Encoder(std::ostream& out) : writer_(out) {
        first_file_ = true;
    }

    Encoder::~Encoder() {
        if (!first_file_) {
            writer_.Write(archive_end_code_, archive_end_code_.size());
        }
    }

    void Encoder::EncodeFile(std::istream& in, const std::string file_name) {
        std::vector<size_t> char_count = GetCharCount(in, file_name);

        HuffmanTree tree;
        std::vector<HuffmanTree::EncodedChar> encoded_chars = tree.GetEncodedChars(char_count);

        WriteOutput(encoded_chars, file_name);
    }

    std::vector<size_t> Encoder::GetCharCount(std::istream& in, const std::string file_name) const {
        std::vector<size_t> char_count(Huffman::SYMBOLS_COUNT, 0);
        for (auto char_value : file_name) {
            ++char_count[static_cast<unsigned char>(char_value)];
        }
        BitReader reader(in);
        Letter char_value = reader.Read();
        while (!reader.EndOfStream()) {
            ++char_count[char_value];
            char_value = reader.Read();
        }
        ++char_count[Huffman::FILENAME_END];
        ++char_count[Huffman::ONE_MORE_FILE];
        ++char_count[Huffman::ARCHIVE_END];
        return char_count;
    }

    void Encoder::WriteOutput(const std::vector<HuffmanTree::EncodedChar>& encoded_chars, const std::string& file_name) {
        if (!first_file_) {
            writer_.Write(one_more_file_code_, one_more_file_code_.size());
        }

        size_t symbols_count = encoded_chars.size();
        std::vector<const HuffmanTree::EncodedChar*> char_ptrs(Huffman::SYMBOLS_COUNT, nullptr);
        for (const auto& encoded_char : encoded_chars) {
            char_ptrs[encoded_char.char_value] = &encoded_char;
        }

        writer_.Write(symbols_count, Huffman::SYMBOL_SIZE);
        for (size_t i = 0; i < symbols_count; ++i) {
            writer_.Write(encoded_chars[i].char_value, Huffman::SYMBOL_SIZE);
        }
        size_t current_len = 1;
        size_t len_count = 0;
        for (size_t i = 0; i < symbols_count; ++i) {
            while (encoded_chars[i].codelen > current_len) {
                writer_.Write(len_count, Huffman::SYMBOL_SIZE);
                len_count = 0;
                ++current_len;
            }
            ++len_count;
        }
        writer_.Write(len_count, Huffman::SYMBOL_SIZE);
        for (auto char_value : file_name) {
            writer_.Write(char_ptrs[static_cast<unsigned char>(char_value)]->code, char_ptrs[static_cast<unsigned char>(char_value)]->codelen);
        }
        writer_.Write(char_ptrs[Huffman::FILENAME_END]->code, char_ptrs[Huffman::FILENAME_END]->codelen);
        std::ifstream in(file_name, std::ios::binary);
        BitReader reader(in);
        Letter read_char = reader.Read(Huffman::BYTE_SIZE);
        while (!reader.EndOfStream()) {
            writer_.Write(char_ptrs[read_char]->code, char_ptrs[read_char]->codelen);
            read_char = reader.Read(Huffman::BYTE_SIZE);
        }
        in.close();

        one_more_file_code_ = char_ptrs[Huffman::ONE_MORE_FILE]->code;
        archive_end_code_ = char_ptrs[Huffman::ARCHIVE_END]->code;
        first_file_ = false;
    }

    Decoder::Decoder(std::istream& in) : reader_(in) {
    }

    void Decoder::DecodeFiles() {
        bool one_more_file = true;
        while (one_more_file) {
            one_more_file = DecodeFile();
        }
    }

    bool Decoder::WriteFile(HuffmanTree& tree) {
        std::string output_file;
        while (true) {
            auto opt_char = tree.NextNode(reader_.Read(1));
            if (opt_char.has_value()) {
                if (opt_char.value() == Huffman::FILENAME_END) {
                    break;
                }
                output_file += static_cast<char>(opt_char.value());
            }
        }
        std::ofstream out(output_file, std::ios::binary);
        BitWriter writer(out);

        bool one_more_file = false;
        while (true) {
            auto opt_char = tree.NextNode(reader_.Read(1));
            if (opt_char.has_value()) {
                if (opt_char.value() == Huffman::ONE_MORE_FILE) {
                    one_more_file = true;
                    break;
                }
                if (opt_char.value() == Huffman::ARCHIVE_END) {
                    break;
                }
                writer.Write(opt_char.value());
            }
        }

        out.close();
        return one_more_file;
    }

    bool Decoder::DecodeFile() {
        size_t symbols_count = reader_.Read(Huffman::SYMBOL_SIZE);
        std::vector<std::pair<size_t, Letter>> char_codelen;
        for (size_t i = 0; i < symbols_count; ++i) {
            char_codelen.push_back({ 0, reader_.Read(Huffman::SYMBOL_SIZE) });
        }
        std::vector<size_t> len_count;
        size_t len_sum = 0;
        while (len_sum < symbols_count) {
            len_count.push_back(reader_.Read(Huffman::SYMBOL_SIZE));
            len_sum += len_count.back();
        }
        size_t char_index = 0;
        for (size_t i = 0; i < len_count.size(); ++i) {
            for (size_t j = 0; j < len_count[i]; ++j) {
                char_codelen[char_index].first = i + 1;
                ++char_index;
            }
        }
        HuffmanTree tree;
        tree.BuildTreeWithLeaves(char_codelen);

        return WriteFile(tree);
    }
}
