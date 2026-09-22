#include "bmplib.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Unknown format. Usage: filter <input.bmp> <output.bmp> <mask_type(4 or 8)>\n";
        return 0;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    int mask_type = std::stoi(argv[3]);
    Image img;
    img.readBMP(input_file);
    bool use8neighbors = (mask_type == 8);
    img.contrast_filter(use8neighbors);
    img.saveBMP(output_file);
    std::cout << "Done";
    return 0;
}