#include "bmplib/Graphics.h"
#include "bmplib/Image.h"
#include <cmath>

void Graphics::draw_line(Image& img, int x1, int y1, int x2, int y2, RGB color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int signX = (x2 > x1) ? 1 : -1; // проверяем в какую сторону двигаться по x
    int signY = (y2 > y1) ? 1 : -1; // проверяем в какую сторону двигаться по y
    int d = dx - dy; // общая ошибка отклонения
    img.set_pixel(x2, y2, color);
    while(x1 != x2 || y1 != y2) {
        img.set_pixel(x1, y1, color);
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