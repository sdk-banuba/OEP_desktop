#ifndef __MAIN_QUEUE_RUNNER_H__
#define __MAIN_QUEUE_RUNNER_H__

#include <functional>

class main_queue_runner
{
public:
    void run_on_main_queue(std::function<void()> f);
};

#endif