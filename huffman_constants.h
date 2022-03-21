#pragma once

#include <vector>

namespace Huffman {
    using Letter = size_t;
    using Code = std::vector<bool>;

    const size_t BYTE_SIZE = 8;
    const size_t SYMBOL_SIZE = 9;
    const size_t SYMBOLS_COUNT = (1 << 8) + 3;

    const Letter FILENAME_END = 256;
    const Letter ONE_MORE_FILE = 257;
    const Letter ARCHIVE_END = 258;
}
