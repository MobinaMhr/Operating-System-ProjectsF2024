#include "thread.hpp"

Thread::Thread() {}

Thread::~Thread() {}

void Thread::get_size()
{
    std::cout << std::to_string(t_ids.size()) << std::endl;
}

void Thread::wait_all()
{
    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);
    t_ids.clear();
}

void Thread::add_task(pthread_t t_id)
{
    t_ids.push_back(t_id);
}
