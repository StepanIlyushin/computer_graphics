#include <iostream>
#include "bmplib/Image.h"
#include "bmplib/Graphics.h"
#include "bmplib/Algorithms.h"

int main() {
    Image img;

    std::cout << "Загрузка изображения..." << std::endl;
    if (!img.readBMP("blank500.bmp")) {
        std::cout << "Ошибка: Не удалось открыть файл blank500.bmp\n";
        return 1;
    }

    RGB line_color = {0, 0, 0};       // Черные границы
    RGB fill_color = {0, 0, 255};     // Красная заливка внутри

    std::cout << "Отрисовка треугольника..." << std::endl;
    Graphics::draw_line(img, 250, 100, 100, 400, line_color);
    Graphics::draw_line(img, 100, 400, 400, 400, line_color);
    Graphics::draw_line(img, 400, 400, 250, 100, line_color);

    std::cout << "Заливка области..." << std::endl;
    Algorithms::start_fill(img, 250, 300, fill_color);

    Algorithms::contrast_filter(img, false);

    std::cout << "Сохранение результата..." << std::endl;
    if (img.saveBMP("result.bmp")) {
        std::cout << "Готово! Результат сохранен в result.bmp\n";
    } else {
        std::cout << "Ошибка при сохранении файла!\n";
    }

    return 0;
}