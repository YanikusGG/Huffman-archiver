#include "bitstream.h"

BitReader::BitReader(std::istream& in) : in_(in) {
    buffer_ = 0;
    buffer_size_ = 0;
    eos_ = false;
}

size_t BitReader::Read(size_t bit_count) {
    if (eos_) {
        throw std::runtime_error("BitReader: Trying to read from end of stream (EOS).");
    }
    size_t data = 0;
    for (size_t current_bit = 0; current_bit < bit_count; ++current_bit) {
        if (buffer_size_ == 0) {
            char temp;
            if (!in_.get(temp)) {
                temp = 0;
                eos_ = true;
            }
            buffer_ = static_cast<unsigned char>(temp);
            buffer_size_ = STANDARD_BIT_COUNT;
        }
        data ^= (((buffer_ >> (STANDARD_BIT_COUNT - buffer_size_)) & 0b1) << current_bit);
        --buffer_size_;
    }
    return data;
}

bool BitReader::EndOfStream() const {
    return eos_;
}

BitWriter::BitWriter(std::ostream& out) : out_(out) {
    buffer_ = 0;
    buffer_size_ = 0;
}

BitWriter::~BitWriter() {
    if (buffer_size_ > 0) {
        out_.write(reinterpret_cast<char*>(&buffer_), 1);
    }
}

void BitWriter::Write(uint64_t data, size_t bit_count) {
    for (size_t current_bit = 0; current_bit < bit_count; ++current_bit) {
        buffer_ ^= (((data >> current_bit) & 0b1) << buffer_size_);
        ++buffer_size_;
        if (buffer_size_ == STANDARD_BIT_COUNT) {
            out_.write(reinterpret_cast<char*>(&buffer_), 1);
            buffer_ = 0;
            buffer_size_ = 0;
        }
    }
}

void BitWriter::Write(std::vector<bool> data, size_t bit_count) {
    for (size_t current_bit = 0; current_bit < bit_count; ++current_bit) {
        buffer_ ^= ((data[current_bit]) << buffer_size_);
        ++buffer_size_;
        if (buffer_size_ == STANDARD_BIT_COUNT) {
            out_.write(reinterpret_cast<char*>(&buffer_), 1);
            buffer_ = 0;
            buffer_size_ = 0;
        }
    }
}
