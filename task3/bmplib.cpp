#include "bmplib.h"
#include <fstream>
#include <iostream>

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

unsigned char check(int val) {
    if (val < 0) {
        return 0;
    }
    if (val > 255) {
        return 255;
    }
    return static_cast<char>(val);
}

void Image::contrast_filter(bool use8pixels) {
    std::vector<std::vector<RGB>> new_pixels = pixels;

    int mask4[3][3] = {{ 0, -1,  0}, {-1,  5, -1},{ 0, -1,  0}};
    int mask8[3][3] = {{-1, -1, -1},{-1,  9, -1},{-1, -1, -1}};
    int (*mask)[3] = use8pixels ? mask8 : mask4;

    // делаем такой проход, тк у первого и последнего пикселя нет маски
    for (int row = 1; row < bmih.height - 1; ++row) {
        for (int col = 1; col < bmih.width - 1; ++col) {
            
            int sumB = 0;
            int sumG = 0;
            int sumR = 0;

            // накладываем маску 3х3
            for (int i = -1;i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int weight = mask[i + 1][j + 1];
                    RGB p = pixels[row + i][col + j];
                    sumB += p.Blue * weight;
                    sumG += p.Green * weight;
                    sumR += p.Red * weight;
                }
            }

            new_pixels[row][col].Blue = check(sumB);
            new_pixels[row][col].Green = check(sumG);
            new_pixels[row][col].Red = check(sumR);
        }
    }

    pixels = new_pixels;
}