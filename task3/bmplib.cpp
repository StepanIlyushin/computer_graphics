#include "bmplib.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <queue>

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

void Image::set_pixel(int x, int y, RGB color) {
    if (bmih.width > x && bmih.height > y && x >= 0 && y >= 0 ) {
            pixels[y][x] = color;
        }
}

void Image::draw_line(int x1, int y1, int x2, int y2, RGB color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int signX = (x2 > x1) ? 1 : -1; // проверяем в какую сторону двигаться по x
    int signY = (y2 > y1) ? 1 : -1; // проверяем в какую сторону двигаться по y
    int d = dx - dy; // общая ошибка отклонения
    set_pixel(x2, y2, color);
    while(x1 != x2 || y1 != y2) {
        set_pixel(x1, y1, color);
        int d2 = d * 2;
        if(d2 > -dy) // проверка на то нужно ли шагнуть по х
        {
            d -= dy;
            x1 += signX;
        }
        if(d2 < dx) // проверка на то нужно ли шагнуть по y
        {
            d += dx;
            y1 += signY;
        }
    }
}

bool is_same_color(RGB c1, RGB c2) {
    return c1.Red == c2.Red && c1.Green == c2.Green && c1.Blue == c2.Blue;
}

void Image::fill_wave(int start_x, int start_y, RGB target_color, RGB fill_color) {
	// 1. Проверяем стартовую точку
    if (start_x < 0 || start_x >= bmih.width || start_y < 0 || start_y >= bmih.height) {
        return;
    }

    // 2. Если цвет уже нужный - ничего не делаем
    if (is_same_color(target_color, fill_color)) {
        return;
    }

    // 3. Если мы кликнули мимо фона - выходим
    if (!is_same_color(pixels[start_y][start_x], target_color)) {
        return;
    }

    // создаем очередь
    std::queue<std::pair<int, int>> q;
    q.push({start_x, start_y});

    // красим стартовый пиксель, чтобы не добавить его в очередь дважды
    set_pixel(start_x, start_y, fill_color);

    while (!q.empty()) {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        // Проверяем 4-х соседей. Если сосед подходит по цвету,
        // сразу красим его и добавляем в очередь.

        // Правый
        if (x + 1 < bmih.width && is_same_color(pixels[y][x + 1], target_color)) {
            set_pixel(x + 1, y, fill_color);
            q.push({x + 1, y});
        }
        // Левый
        if (x - 1 >= 0 && is_same_color(pixels[y][x - 1], target_color)) {
            set_pixel(x - 1, y, fill_color);
            q.push({x - 1, y});
        }
        // Верхний
        if (y - 1 >= 0 && is_same_color(pixels[y - 1][x], target_color)) {
            set_pixel(x, y - 1, fill_color);
            q.push({x, y - 1});
        }
        // Нижний
        if (y + 1 < bmih.height && is_same_color(pixels[y + 1][x], target_color)) {
            set_pixel(x, y + 1, fill_color);
            q.push({x, y + 1});
        }
    }
}

void Image::start_fill(int x, int y, RGB fill_color) {
    if (x >= 0 && x < bmih.width && y >= 0 && y < bmih.height) {
        RGB target_color = pixels[y][x];
        fill_wave(x, y, target_color, fill_color);
    }
}