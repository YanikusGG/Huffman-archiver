#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <vector>

const size_t STANDARD_BIT_COUNT = 8;

class BitReader {
public:
    explicit BitReader(std::istream& in);

    size_t Read(size_t bit_count = STANDARD_BIT_COUNT);
    bool EndOfStream() const;

private:
    std::istream& in_;
    size_t buffer_;
    size_t buffer_size_;
    bool eos_;
};

class BitWriter {
public:
    explicit BitWriter(std::ostream& out);
    ~BitWriter();

    void Write(uint64_t data, size_t bit_count = STANDARD_BIT_COUNT);
    void Write(std::vector<bool> data, size_t bit_count = STANDARD_BIT_COUNT);

private:
    std::ostream& out_;
    size_t buffer_;
    size_t buffer_size_;
};
