#include "bmp.hpp"

BMP::BMP(const BMP &other)
    : pixels_(other.pixels_),
      bitmap_file_header_(other.bitmap_file_header_),
      bitmap_info_header_(other.bitmap_info_header_),
      t_pool_(other.t_pool_),
      thread_count_(other.thread_count_) {}

BMP::BMP(int thread_count, Thread *t_pool)
    : thread_count_(thread_count), t_pool_(t_pool) {}

int BMP::height() const { return bitmap_info_header_.biHeight; }

int BMP::width() const { return bitmap_info_header_.biWidth; }

BMP::Pixel &BMP::operator()(int row, int col) { return pixels_[row][col]; }

const BMP::Pixel &BMP::operator()(int row, int col) const { return pixels_[row][col]; }

std::istream &BMP::read(std::istream &read_stream)
{
    r_headers(read_stream);
    r_bitmap(read_stream);
    return read_stream;
}

std::ostream &BMP::write(std::ostream &write_stream)
{
    w_headers(write_stream);
    w_bitmap(write_stream);
    return write_stream;
}

void BMP::r_headers(std::istream &read_stream)
{
    read_stream.read(
        (char *)(&bitmap_file_header_),
        sizeof(BITMAPFILEHEADER));

    if (bitmap_file_header_.bfType != 0x4D42)
        throw std::runtime_error("Not a bmp file.");

    read_stream.read(
        (char *)(&bitmap_info_header_),
        sizeof(BITMAPINFOHEADER));
}

int BMP::pixel_pos(int row, int col)
{
    int row_len = std::ceil((3 * bitmap_info_header_.biWidth) / 4) * 4;
    int height = bitmap_info_header_.biHeight - row - 1;
    int width = col * 3;
    int result = height * row_len + width;
    return result;
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

void BMP::process_bitmap(void *(*func)(void *), unsigned char *img_buff)
{
    auto partialize = [&img_buff, this, func](int begin_point, int end_point)
    {
        BMP::FileArgs *args = new BMP::FileArgs;
        args->bmp = this;
        args->begin_point = begin_point;
        args->end_point = end_point;
        args->img_buff = img_buff;
        pthread_t t_id;
        pthread_create(&t_id, NULL, func, (void *)args);
        return t_id;
    };

    for (int i = 0; i < thread_count_; ++i)
    {
        int begin_point = i * (bitmap_info_header_.biHeight / thread_count_);
        int end_point = (i + 1) * (bitmap_info_header_.biHeight / thread_count_);
        t_pool_->add_task(partialize(begin_point, end_point));
    }

    int begin_point = (bitmap_info_header_.biHeight - (bitmap_info_header_.biHeight % thread_count_));
    int end_point = bitmap_info_header_.biHeight;
    t_pool_->add_task(partialize(begin_point, end_point));

    t_pool_->wait_all();
}

void BMP::r_bitmap(std::istream &read_stream)
{
    read_stream.seekg(bitmap_file_header_.bfOffBits, std::ios::beg);
    pixels_.assign(bitmap_info_header_.biHeight, std::vector<Pixel>(bitmap_info_header_.biWidth));

    unsigned char *img_buff = new unsigned char[bitmap_info_header_.biSizeImage];
    read_stream.read((char *)img_buff, bitmap_info_header_.biSizeImage);

    process_bitmap(read_partial, img_buff);

    delete[] img_buff;
}

void BMP::w_bitmap(std::ostream &write_stream)
{
    unsigned char *img_buff = new unsigned char[bitmap_info_header_.biSizeImage];

    process_bitmap(w_partial, img_buff);

    write_stream.write((char *)(img_buff), bitmap_info_header_.biSizeImage);

    delete[] img_buff;
}

void *BMP::read_partial(void *args)
{
    BMP::FileArgs *pargs = static_cast<BMP::FileArgs *>(args);
    auto &begin_point = pargs->begin_point;
    auto &end_point = pargs->end_point;
    auto &bmp = pargs->bmp;
    auto &img_buff = pargs->img_buff;

    for (int i = begin_point; i < end_point; ++i)
    {
        for (int j = 0; j < bmp->bitmap_info_header_.biWidth; ++j)
        {
            int pos = bmp->pixel_pos(i, j);
            bmp->pixels_[i][j] = {img_buff[pos + 2], img_buff[pos + 1], img_buff[pos]};
        }
    }

    delete pargs;
    return NULL;
}

void *BMP::w_partial(void *args)
{
    BMP::FileArgs *pargs = static_cast<BMP::FileArgs *>(args);
    auto &begin_point = pargs->begin_point;
    auto &end_point = pargs->end_point;
    auto &bmp = pargs->bmp;
    auto &img_buff = pargs->img_buff;

    for (int i = 0; i < bmp->bitmap_info_header_.biHeight; ++i)
    {
        for (int j = 0; j < bmp->bitmap_info_header_.biWidth; ++j)
        {
            int pos = bmp->pixel_pos(i, j);
            img_buff[pos] = bmp->pixels_[i][j].blue;
            img_buff[pos + 1] = bmp->pixels_[i][j].green;
            img_buff[pos + 2] = bmp->pixels_[i][j].red;
        }
    }

    delete pargs;
    return NULL;
}
