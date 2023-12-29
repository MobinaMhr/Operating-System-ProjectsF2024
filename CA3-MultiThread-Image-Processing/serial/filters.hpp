#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "bmp.hpp"

struct FilterArgs
{
    FilterArgs(BMP &bmp_, const BMP &bmp_fer_)
        : bmp(bmp_), original_bmp(bmp_fer_) {}

    BMP &bmp;
    const BMP &original_bmp;
};

void *apply_purple_haze(void *args);

void *apply_blur(void *args);

void *apply_vertical_mirror(void *args);

void drawLine(BMP &bmp, int x1, int y1, int x2, int y2);

void diagonal_hatch_pattern(BMP &bmp);

class Filter
{
public:
    Filter(BMP &input_pic);

    void apply_filter(void *(*filter_func)(void *));
    void apply_pattern();

private:
    BMP &pic_;
    void *(*filter_func_)(void *);
};
#endif
