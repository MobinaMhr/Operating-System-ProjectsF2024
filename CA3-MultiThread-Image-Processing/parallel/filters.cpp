#include "filters.hpp"

static const float blur_kernel[3][3] = {
    {1.0 / 16, 2.0 / 16, 1.0 / 16},
    {2.0 / 16, 4.0 / 16, 2.0 / 16},
    {1.0 / 16, 2.0 / 16, 1.0 / 16}};

void *apply_partial_purple_haze(void *args)
{
    FilterArgs *pargs = static_cast<FilterArgs *>(args);
    auto &begin_point = pargs->begin_point;
    auto &end_point = pargs->end_point;
    auto &bmp = pargs->bmp;
    auto &original_bmp = pargs->original_bmp;

    for (int col = begin_point; col < end_point; ++col)
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

void *apply_partial_blur(void *args)
{
    FilterArgs *pargs = static_cast<FilterArgs *>(args);
    auto &begin_point = pargs->begin_point;
    auto &end_point = pargs->end_point;
    auto &bmp = pargs->bmp;
    auto &original_bmp = pargs->original_bmp;

    for (int col = begin_point; col < end_point; ++col)
    {
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

void *apply_partial_vertical_mirror(void *args)
{
    FilterArgs *pargs = static_cast<FilterArgs *>(args);
    auto &begin_point = pargs->begin_point;
    auto &end_point = pargs->end_point;
    auto &bmp = pargs->bmp;
    auto &original_bmp = pargs->original_bmp;

    for (int col = begin_point; col < end_point; ++col)
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

void *drawLine(void *args)
{
    DrawLineArgs *pargs = static_cast<DrawLineArgs *>(args);

    int dx = pargs->x2 - pargs->x1;
    int dy = pargs->y2 - pargs->y1;
    int steps = std::max(abs(dx), abs(dy));
    float xIncrement = static_cast<float>(dx) / (float)steps;
    float yIncrement = static_cast<float>(dy) / (float)steps;
    auto x_amount = static_cast<float>(pargs->x1);
    auto y_amount = static_cast<float>(pargs->y1);
    for (int i = 0; i <= steps; ++i)
    {
        pargs->bmp((int)y_amount, (int)x_amount) = WHITE;
        x_amount += xIncrement;
        y_amount += yIncrement;
    }

    return NULL;
}

Filter::Filter(BMP &input_pic, Thread *t_pool)
    : pic_(input_pic), t_pool_(t_pool) {}

void Filter::apply_pattern(int thread_num)
{
    auto partialize = [this](int x1, int y1, int x2, int y2)
    {
        DrawLineArgs *args = new DrawLineArgs(
            this->pic_, x1, y1, x2, y2);
        pthread_t t_id;
        pthread_create(&t_id, NULL, drawLine, (void *)args);
        return t_id;
    };

    int mid_width = pic_.width() / 2;
    int mid_height = pic_.height() / 2;

    t_pool_->add_task(partialize(0, mid_height, mid_width, 0));
    t_pool_->add_task(partialize(0, pic_.height() - 1, pic_.width(), 0));
    // t_pool_->add_task(partialize(0, pic_.height() - 1, mid_width, mid_height));
    // t_pool_->add_task(partialize(mid_width, mid_height, pic_.width(), 0));
    t_pool_->add_task(partialize(pic_.width() - 1, mid_height, mid_width, pic_.height() - 1));
    t_pool_->wait_all();
}

void Filter::apply_filter(int thread_num, void *(*filter_func)(void *))
{
    BMP pic_cpy(pic_);

    auto partialize = [&pic_cpy, this, &filter_func](int begin_point, int end_point)
    {
        FilterArgs *args = new FilterArgs(
            pic_cpy,
            this->pic_,
            begin_point,
            end_point);
        pthread_t t_id;
        pthread_create(&t_id, NULL, filter_func, (void *)args);
        return t_id;
    };

    int width = pic_.width() - 1;

    for (int i = 0; i < thread_num; ++i)
    {
        int begin_point = i * (width / thread_num);
        int end_point = (i + 1) * (width / thread_num);
        t_pool_->add_task(partialize(begin_point, end_point));
    }

    int begin_point = (width - (width % thread_num));
    int end_point = width;
    t_pool_->add_task(partialize(begin_point, end_point));
    t_pool_->wait_all();

    pic_ = pic_cpy;
}
