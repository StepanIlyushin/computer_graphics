#pragma once
#include "bmplib/Image.h"

namespace Algorithms {

    // Фильтр контраста
    void contrast_filter(Image& img, bool use8pixels);

    // Волновой алгоритм заливки
    void fill_wave(Image& img, int start_x, int start_y, RGB target_color, RGB fill_color);

    // Обертка для запуска заливки
    void start_fill(Image& img, int x, int y, RGB fill_color);
}