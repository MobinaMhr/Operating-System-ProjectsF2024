#ifndef BMP_HPP
#define BMP_HPP

#include <iostream>
#include <vector>

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

class BMP
{
#pragma pack(push, 1)
    typedef struct tagBITMAPFILEHEADER
    {
        WORD bfType;
        DWORD bfSize;
        WORD bfReserved1;
        WORD bfReserved2;
        DWORD bfOffBits;
    } BITMAPFILEHEADER;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct tagBITMAPINFOHEADER
    {
        DWORD biSize;
        LONG biWidth;
        LONG biHeight;
        WORD biPlanes;
        WORD biBitCount;
        DWORD biCompression;
        DWORD biSizeImage;
        LONG biXPelsPerMeter;
        LONG biYPelsPerMeter;
        DWORD biClrUsed;
        DWORD biClrImportant;
    } BITMAPINFOHEADER;
#pragma pack(pop)

public:
    struct Pixel
    {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    };

    BMP() = default;
    BMP(const BMP &other);

    int height() const;
    int width() const;

    std::istream &read(std::istream &stream);
    std::ostream &write(std::ostream &stream);

    BMP::Pixel &operator()(int row, int col);
    const BMP::Pixel &operator()(int row, int col) const;

private:
    std::vector<std::vector<Pixel>> pixels_;
    BITMAPFILEHEADER bitmap_file_header_;
    BITMAPINFOHEADER bitmap_info_header_;

    void r_headers(std::istream &read_stream);
    void r_bitmap(std::istream &read_stream);

    void w_headers(std::ostream &write_stream);
    void w_bitmap(std::ostream &write_stream);

    int pixel_pos(int row, int col);
};

const BMP::Pixel WHITE = {(unsigned char)255, (unsigned char)255, (unsigned char)255};

#endif
