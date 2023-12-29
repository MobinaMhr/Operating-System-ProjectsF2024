#include "filters.hpp"

#include <algorithm>
#include <cmath>

#include "bmp.hpp"

static const float blur_kernel[3][3] = {
    {1.0 / 16, 2.0 / 16, 1.0 / 16},
    {2.0 / 16, 4.0 / 16, 2.0 / 16},
    {1.0 / 16, 2.0 / 16, 1.0 / 16}};

void *apply_purple_haze(void *args)
{
    FilterArgs *pargs = static_cast<FilterArgs *>(args);

    auto &bmp = pargs->bmp;
    auto &original_bmp = pargs->original_bmp;

    for (int col = 0; col < bmp.width(); ++col)
    {
        for (int row = 1; row < bmp.height() - 1; ++row)
        {
            float red = 0, blue = 0, green = 0;

            red = (0.5 * bmp(row, col).red + 0.3 * bmp(row, col).green + 0.5 * bmp(row, col).blue);
            green = (0.16 * bmp(row, col).red + 0.5 * bmp(row, col).green + 0.16 * bmp(row, col).blue);
            blue = (0.6 * bmp(row, col).red + 0.2 * bmp(row, col).green + 0.8 * bmp(row, col).blue);

            bmp(row, col).red = (unsigned char)std::min(std::max<int>(red, 0), 255);
            bmp(row, col).blue = (unsigned char)std::min(std::max<int>(blue, 0), 255);
            bmp(row, col).green = (unsigned char)std::min(std::max<int>(green, 0), 255);
        }
    }
    return NULL;
}

void *apply_blur(void *args)
{
    FilterArgs *pargs = static_cast<FilterArgs *>(args);

    auto &bmp = pargs->bmp;
    auto &original_bmp = pargs->original_bmp;
    for (int col = 0; col < bmp.width(); ++col)
    {
        if (col == original_bmp.width() || col == 0)
            continue;
        for (int row = 1; row < bmp.height() - 1; ++row)
        {
            float red = 0, blue = 0, green = 0;

            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    int cur_row = row + i;
                    int cur_col = col + j;
                    const auto &temp = original_bmp(cur_row, cur_col);
                    red += (temp.red) * blur_kernel[i + 1][j + 1];
                    blue += (temp.blue) * blur_kernel[i + 1][j + 1];
                    green += (temp.green) * blur_kernel[i + 1][j + 1];
                }
            }

            bmp(row, col).red = (unsigned char)std::min(std::max<int>(red, 0), 255);
            bmp(row, col).blue = (unsigned char)std::min(std::max<int>(blue, 0), 255);
            bmp(row, col).green = (unsigned char)std::min(std::max<int>(green, 0), 255);
        }
    }
    return NULL;
}

void *apply_vertical_mirror(void *args)
{
    FilterArgs *pargs = static_cast<FilterArgs *>(args);

    auto &bmp = pargs->bmp;
    auto &original_bmp = pargs->original_bmp;

    for (int col = 0; col < bmp.width(); ++col)
    {
        for (int row = 0; row < std::floor(bmp.height() / 2); ++row)
        {
            BMP::Pixel temp = bmp(row, col);
            bmp(row, col) = bmp(bmp.height() - row - 1, col);
            bmp(bmp.height() - row - 1, col) = temp;
        }
    }
    return NULL;
}

void drawLine(BMP &bmp, int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int step_num = std::max(abs(dx), abs(dy));

    float xIncrement = static_cast<float>(dx) / (float)step_num;
    float yIncrement = static_cast<float>(dy) / (float)step_num;

    auto x_amount = static_cast<float>(x1);
    auto y_amount = static_cast<float>(y1);
    for (int i = 0; i <= step_num; ++i)
    {
        bmp((int)y_amount, (int)x_amount) = WHITE;
        x_amount += xIncrement;
        y_amount += yIncrement;
    }
}

void diagonal_hatch_pattern(BMP &bmp)
{
    int mid_width = bmp.width() / 2;
    int mid_height = bmp.height() / 2;

    drawLine(bmp, 0, mid_height, mid_width, 0);
    drawLine(bmp, 0, bmp.height() - 1, bmp.width(), 0);
    drawLine(bmp, bmp.width() - 1, mid_height, mid_width, bmp.height() - 1);
}

Filter::Filter(BMP &input_pic)
    : pic_(input_pic) {}

void Filter::apply_filter(void *(*filter_func)(void *))
{
    BMP pic_cpy(pic_);

    FilterArgs *args = new FilterArgs(
        pic_cpy, this->pic_);

    filter_func(args);
    delete args;

    pic_ = pic_cpy;
}

void Filter::apply_pattern()
{
    int mid_width = pic_.width() / 2;
    int mid_height = pic_.height() / 2;

    drawLine(pic_, 0, mid_height, mid_width, 0);
    drawLine(pic_, 0, pic_.height() - 1, pic_.width(), 0);
    drawLine(pic_, pic_.width() - 1, mid_height, mid_width, pic_.height() - 1);
}