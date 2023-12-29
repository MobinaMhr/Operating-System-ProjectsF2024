#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <iomanip>
#include <unistd.h>

#include "bmp.hpp"
#include "filters.hpp"

int THREAD_COUNT = 4;
int TEST_ITERATIONS = 1;
const std::string OUTPUT_FILENAME = "parallel-output.bmp";

using Durationo_time = std::chrono::duration<float, std::milli>;

Durationo_time apply_filter(Filter &filter, void *(*filter_func)(void *))
{
    auto start_clk = std::chrono::high_resolution_clock::now();

    filter.apply_filter(THREAD_COUNT - 1, filter_func);

    auto end_clk = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        end_clk - start_clk);

    return time_taken;
}

Durationo_time apply_pattern(Filter &filter)
{
    auto start_clk = std::chrono::high_resolution_clock::now();

    filter.apply_pattern(THREAD_COUNT - 1);

    auto end_clk = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        end_clk - start_clk);

    return time_taken;
}

void log(const std::string title, const double record)
{
    std::cout << std::left << std::setw(33) << title << std::setw(10)
              << std::fixed << std::setprecision(4) << record << "ms\n";
}

int main(int argc, char const *argv[])
{

    if (argc < 2)
    {
        std::cerr << "[Usage] " << argv[0] << "<input-file>" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc > 2)
        TEST_ITERATIONS = std::stoi(argv[2]);

    int thread_count = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
    if (argc > 3)
    {
        THREAD_COUNT = std::stoi(argv[3]);
        THREAD_COUNT = (THREAD_COUNT != thread_count)
                           ? thread_count
                           : THREAD_COUNT;
    }

    try
    {
        Thread thread_pool;
        std::string file_name(argv[1]);
        BMP bmp(THREAD_COUNT - 1, &thread_pool);

        auto start_clk = std::chrono::high_resolution_clock::now();
        std::ifstream input_file(file_name);
        bmp.read(input_file);
        input_file.close();
        auto end_clk = std::chrono::high_resolution_clock::now();

        double read_time =
            std::chrono::duration_cast<
                std::chrono::duration<
                    float, std::milli>>(
                end_clk - start_clk)
                .count();

        Filter filter(bmp, &thread_pool);

        double blur_time, purple_time, mirror_time, pattern_time, total_duration;
        for (int i = 0; i < TEST_ITERATIONS; i++)
        {
            blur_time += apply_filter(filter, apply_partial_blur).count();
            purple_time += apply_filter(filter, apply_partial_purple_haze).count();
            mirror_time += apply_filter(filter, apply_partial_vertical_mirror).count();
            pattern_time += apply_pattern(filter).count();
        }

        auto start_clk2 = std::chrono::high_resolution_clock::now();
        std::ofstream output_file(OUTPUT_FILENAME);
        bmp.write(output_file);
        output_file.close();
        auto end_clk2 = std::chrono::high_resolution_clock::now();

        double write_time =
            std::chrono::duration_cast<
                std::chrono::duration<
                    float, std::milli>>(
                end_clk2 - start_clk2)
                .count();

        blur_time /= TEST_ITERATIONS;
        blur_time /= TEST_ITERATIONS;
        blur_time /= TEST_ITERATIONS;
        total_duration = blur_time + purple_time + mirror_time +
                         pattern_time + read_time + write_time;

        log("Read Execution Time: ", read_time);
        log("Blur Filter Execution Time: ", blur_time);
        log("Purple Filter Execution Time: ", purple_time);
        log("Mirror Filter Execution Time: ", mirror_time);
        log("Pattern Filter Execution Time: ", pattern_time);
        // log("Write Execution Time: ", write_time);
        log("Avg Total Execution Time: ", total_duration);

        return EXIT_SUCCESS;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}