#ifndef THREAD_HPP
#define THREAD_HPP

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>

class Thread
{
private:
    std::vector<pthread_t> t_ids;

public:
    Thread();

    ~Thread();

    void get_size();

    void wait_all();

    void add_task(pthread_t t_id);
};

#endif