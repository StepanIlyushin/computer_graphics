#include "bmplib/Algorithms.h"
#include "bmplib/Image.h"
#include <queue>

unsigned char check(int val) {
    if (val < 0) {
        return 0;
    }
    if (val > 255) {
        return 255;
    }
    return static_cast<char>(val);
}

bool is_same_color(RGB c1, RGB c2) {
    return c1.Red == c2.Red && c1.Green == c2.Green && c1.Blue == c2.Blue;
}

void Algorithms::contrast_filter(Image& img, bool use8pixels) {
    int width = img.get_width();
    int height = img.get_height();

    // Создаем копию пикселей для фильтрации
    std::vector<std::vector<RGB>> new_pixels(height, std::vector<RGB>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            new_pixels[y][x] = img.get_pixel(x, y);
        }
    }

    int mask4[3][3] = {{ 0, -1,  0}, {-1,  5, -1},{ 0, -1,  0}};
    int mask8[3][3] = {{-1, -1, -1},{-1,  9, -1},{-1, -1, -1}};
    int (*mask)[3] = use8pixels ? mask8 : mask4;

    // делаем такой проход, тк у первого и последнего пикселя нет маски
    for (int row = 1; row < height - 1; ++row) {
        for (int col = 1; col < width - 1; ++col) {

            int sumB = 0;
            int sumG = 0;
            int sumR = 0;

            // накладываем маску 3х3
            for (int i = -1;i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int weight = mask[i + 1][j + 1];
                    RGB p = img.get_pixel(col + j, row + i);
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

    // Возвращаем измененные пиксели обратно в картинку
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            img.set_pixel(x, y, new_pixels[y][x]);
        }
    }
}

void Algorithms::fill_wave(Image& img, int start_x, int start_y, RGB target_color, RGB fill_color) {
    // 1. Проверяем стартовую точку
    if (start_x < 0 || start_x >= img.get_width() || start_y < 0 || start_y >= img.get_height()) {
        return;
    }

    // 2. Если цвет уже нужный - ничего не делаем
    if (is_same_color(target_color, fill_color)) {
        return;
    }

    // 3. Если мы кликнули мимо фона - выходим
    if (!is_same_color(img.get_pixel(start_x, start_y), target_color)) {
        return;
    }

    // создаем очередь
    std::queue<std::pair<int, int>> q;
    q.push({start_x, start_y});

    // красим стартовый пиксель, чтобы не добавить его в очередь дважды
    img.set_pixel(start_x, start_y, fill_color);

    while (!q.empty()) {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        // Проверяем 4-х соседей. Если сосед подходит по цвету,
        // сразу красим его и добавляем в очередь.

        // Правый
        if (x + 1 < img.get_width() && is_same_color(img.get_pixel(x + 1, y), target_color)) {
            img.set_pixel(x + 1, y, fill_color);
            q.push({x + 1, y});
        }
        // Левый
        if (x - 1 >= 0 && is_same_color(img.get_pixel(x - 1, y), target_color)) {
            img.set_pixel(x - 1, y, fill_color);
            q.push({x - 1, y});
        }
        // Верхний
        if (y - 1 >= 0 && is_same_color(img.get_pixel(x, y - 1), target_color)) {
            img.set_pixel(x, y - 1, fill_color);
            q.push({x, y - 1});
        }
        // Нижний
        if (y + 1 < img.get_height() && is_same_color(img.get_pixel(x, y + 1), target_color)) {
            img.set_pixel(x, y + 1, fill_color);
            q.push({x, y + 1});
        }
    }
}

void Algorithms::start_fill(Image& img, int x, int y, RGB fill_color) {
    if (x >= 0 && x < img.get_width() && y >= 0 && y < img.get_height()) {
        RGB target_color = img.get_pixel(x, y);
        fill_wave(img, x, y, target_color, fill_color);
    }
}