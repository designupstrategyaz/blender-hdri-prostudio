#include "multithreading.h"

#include <vector>
#include <thread>

using namespace std;

namespace Multithreading
{
    vector<thread*> _threads;
}

void Multithreading::init()
{
    _threads.resize(thread::hardware_concurrency());
}

void Multithreading::exec(func task, const void *arg1, void *arg2)
{
    for(int i = 0; i < threads(); i++)
        _threads[i] = new thread(task, i, arg1, arg2);

    for(int i = 0; i < threads(); i++) {
        _threads[i]->join();
        delete _threads[i];
    }
}

int Multithreading::threads()
{
    return _threads.size();
}
