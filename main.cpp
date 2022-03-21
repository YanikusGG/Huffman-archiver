#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "coder.h"

void CreateArchive(int file_count, const char* file_names[]) {
    std::ofstream out(std::string(file_names[2]), std::ios::binary);
    Huffman::Encoder encoder(out);
    for (int i = 3; i < file_count; ++i) {
        std::ifstream in(std::string(file_names[i]), std::ios::binary);
        encoder.EncodeFile(in, std::string(file_names[i]));
        in.close();
    }
    std::cout << "Archive created successfully!" << std::endl;
}

void ExtractFiles(const char* archive_name) {
    std::ifstream in(std::string(archive_name), std::ios::binary);
    Huffman::Decoder decoder(in);
    decoder.DecodeFiles();
    in.close();
    std::cout << "Files extracted successfully!" << std::endl;
}

void PrintHelp() {
    std::cout << "HELP:" << std::endl;
    std::cout << "-c archive_name file1 [file2 ...] - to archive files file1, file2, ... and save result in file archive_name." << std::endl;
    std::cout << "-d archive_name - unarchive files from archive archive_name and put in the current directory." << std::endl;
    std::cout << "-h - to display help on using the program." << std::endl;
}

void InvalidInput() {
    std::cout << "Invalid input. Please use -h to display help on using the program." << std::endl;
}

int main(int argc, const char* argv[]) {
    try {
        if (argc >= 4 && std::string(argv[1]) == "-c") {
            CreateArchive(argc, argv);
        } else if (argc >= 3 && std::string(argv[1]) == "-d") {
            ExtractFiles(argv[2]);
        } else if (argc >= 2 && std::string(argv[1]) == "-h") {
            PrintHelp();
        } else {
            InvalidInput();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error!" << std::endl << e.what() << std::endl;
        return 1;
    }

    return 0;
}
