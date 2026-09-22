#pragma once 
#include <cstdint>
#include <vector>

#pragma pack(push, 1)

struct BITMAPFILEHEADER{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset_data; // cмещение до самого массива пикселей
};

struct BITMAPINFOHEADER {
    uint32_t size; //размер структуры в байтах
    int32_t width; //ширина в пикселях 
    int32_t height; // высота в пикселях
    uint16_t planes; // всегда должно быть 1
    uint16_t bit_count; // кол во бит на цвет 
    uint32_t compression; // есть или нет сжатие
    uint32_t size_image; // размер изображения 
    int32_t x_pixels_per_meter; // горизонтальное разрешение 
    int32_t y_pixels_per_meter; // вертикальное разрешение
    uint32_t colors_used; // колво цветов
    uint32_t colors_important; // кол во существующих цветов 
};

struct RGB {
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
};

#pragma pack(pop)

class Image {
private:
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    std::vector<std::vector<RGB>> pixels; 

public:
    bool readBMP(const char* fname);
    bool saveBMP(const char* fname);
    void contrast_filter(bool use8pixels);
};

