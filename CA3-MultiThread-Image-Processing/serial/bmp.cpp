#include "bmp.hpp"

#include <string.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

BMP::BMP(const BMP &other)
    : pixels_(other.pixels_),
      bitmap_file_header_(other.bitmap_file_header_),
      bitmap_info_header_(other.bitmap_info_header_) {}

int BMP::height() const { return bitmap_info_header_.biHeight; }

int BMP::width() const { return bitmap_info_header_.biWidth; }

BMP::Pixel &BMP::operator()(int row, int col) { return pixels_[row][col]; }

const BMP::Pixel &BMP::operator()(int row, int col) const { return pixels_[row][col]; }

std::istream &BMP::read(std::istream &stream)
{
    r_headers(stream);
    r_bitmap(stream);
    return stream;
}

std::ostream &BMP::write(std::ostream &stream)
{
    w_headers(stream);
    w_bitmap(stream);
    return stream;
}

void BMP::r_headers(std::istream &read_stream)
{
    read_stream.read((char *)(&bitmap_file_header_), sizeof(BITMAPFILEHEADER));

    if (bitmap_file_header_.bfType != 0x4D42)
        throw std::runtime_error("Invalid bmp file.");

    read_stream.read((char *)(&bitmap_info_header_), sizeof(BITMAPINFOHEADER));
}

void BMP::r_bitmap(std::istream &read_stream)
{
    read_stream.seekg(bitmap_file_header_.bfOffBits, std::ios::beg);
    pixels_.assign(bitmap_info_header_.biHeight, std::vector<Pixel>(bitmap_info_header_.biWidth));

    unsigned char *img_buff = new unsigned char[bitmap_info_header_.biSizeImage];
    read_stream.read((char *)img_buff, bitmap_info_header_.biSizeImage);

    for (int i = 0; i < bitmap_info_header_.biHeight; ++i)
    {
        for (int j = 0; j < bitmap_info_header_.biWidth; ++j)
        {
            int pos = pixel_pos(i, j);
            pixels_[i][j] = {img_buff[pos + 2], img_buff[pos + 1], img_buff[pos]};
        }
    }
    delete[] img_buff;
}

void BMP::w_headers(std::ostream &write_stream)
{
    write_stream.write((char *)(&bitmap_file_header_), sizeof(BITMAPFILEHEADER));
    write_stream.write((char *)(&bitmap_info_header_), sizeof(BITMAPINFOHEADER));

    int cur_stream_point = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    while (cur_stream_point < bitmap_file_header_.bfOffBits)
    {
        write_stream << '\0';
        ++cur_stream_point;
    }
}

void BMP::w_bitmap(std::ostream &write_stream)
{
    char *img_buff = new char[bitmap_info_header_.biSizeImage];
    for (int i = 0; i < bitmap_info_header_.biHeight; ++i)
    {
        for (int j = 0; j < bitmap_info_header_.biWidth; ++j)
        {
            int pos = pixel_pos(i, j);
            img_buff[pos] = pixels_[i][j].blue;
            img_buff[pos + 1] = pixels_[i][j].green;
            img_buff[pos + 2] = pixels_[i][j].red;
        }
    }
    write_stream.write(img_buff, bitmap_info_header_.biSizeImage);
    delete[] img_buff;
}

int BMP::pixel_pos(int row, int col)
{
    int row_len = std::ceil((3 * bitmap_info_header_.biWidth) / 4) * 4;
    int height = bitmap_info_header_.biHeight - row - 1;
    int width = col * 3;
    int result = height * row_len + width;
    return result;
}
