#include "bmplib/Image.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

bool Image::readBMP(const char* fname) {
    std::ifstream inFile(fname, std::ios_base::binary);
    if (!inFile) {
        std::cout << "Can not open input file" << std::endl;
        return false;
    }
    inFile.read(reinterpret_cast<char*>(&bmfh), sizeof(bmfh));
    inFile.read(reinterpret_cast<char*>(&bmih), sizeof(bmih));

    // 1) проверяем сигнатуру BM 2) проверяем битность, тк каждый цвет по 8 бит, то всего 24  
    if (bmfh.type != 256*int('M') + int('B') || bmih.bit_count != 24 || bmih.compression != 0){
        inFile.close();
        return false;
    }

    pixels.resize(bmih.height);
    inFile.seekg(bmfh.offset_data, std::ios_base::beg);
    for (int i = 0; i < bmih.height; ++i) {
        pixels[i].resize(bmih.width);
    }
    const int offset = (bmih.width % 4) ? (4 - (3 * bmih.width) % 4) : 0;
    for (int row = bmih.height - 1; row >= 0; --row) {
        for (int col = 0; col < bmih.width; ++col) {
            inFile.read(reinterpret_cast<char*>(&pixels[row][col]), sizeof(RGB));
        }
        if (offset > 0) {
            inFile.seekg(offset, std::ios_base::cur);
        }
    }

    inFile.close();
    return true;
}

bool Image::saveBMP(const char* fname) {
    std::ofstream outFile(fname, std::ios_base::binary);
    if (!outFile) {
        std::cout << "Can not open output file" << std::endl;
        return false;
    }
    outFile.write(reinterpret_cast<char*>(&bmfh), sizeof(bmfh));
    outFile.write(reinterpret_cast<char*>(&bmih), sizeof(bmih));

    const int offset = (bmih.width % 4) ? (4 - (3 * bmih.width) % 4) : 0;

    unsigned char trash[3] = {0, 0, 0};
    for (int row = bmih.height - 1; row >= 0; --row) {
        for (int col = 0; col < bmih.width; ++col) {
            outFile.write(reinterpret_cast<char*>(&pixels[row][col]), sizeof(RGB));
        }
        if (offset > 0) {
            outFile.write(reinterpret_cast<char*>(trash), offset);
        }
    }

    outFile.close();
    return true;
}

void Image::set_pixel(int x, int y, RGB color) {
    if (bmih.width > x && bmih.height > y && x >= 0 && y >= 0 ) {
        pixels[y][x] = color;
    }
}

