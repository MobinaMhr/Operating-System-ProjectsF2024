#ifndef FILTERS_HPP
#define FILTERS_HPP

#include <algorithm>
#include <cmath>

#include "bmp.hpp"

struct FilterArgs
{
    FilterArgs(BMP &bmp_, const BMP &bmp_fer_, int row_begin_, int row_end_)
        : bmp(bmp_), original_bmp(bmp_fer_), begin_point(row_begin_),
          end_point(row_end_) {}

    BMP &bmp;
    const BMP &original_bmp;
    int begin_point;
    int end_point;
};

struct DrawLineArgs
{
    DrawLineArgs(BMP &bmp_, int x1_, int y1_, int x2_, int y2_)
        : bmp(bmp_), x1(x1_), y1(y1_), x2(x2_), y2(y2_) {}

    BMP &bmp;
    int x1, y1, x2, y2;
};

void *apply_partial_purple_haze(void *args);

void *apply_partial_blur(void *args);

void *apply_partial_vertical_mirror(void *args);

void *drawLine(void *args);

class Filter
{
public:
    Filter(BMP &input_pic, Thread *t_pool);

    void apply_filter(int thread_num, void *(*filter_func)(void *));
    void apply_pattern(int thread_num);

private:
    Thread *t_pool_;
    BMP &pic_;
};
#endif
