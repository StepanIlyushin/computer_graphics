#pragma once
#include "bmplib/Image.h"

namespace Graphics {
    // Алгоритм Брезенхема
    void draw_line(Image& img, int x1, int y1, int x2, int y2, RGB color);
}