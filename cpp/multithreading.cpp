#include "multithreading.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

namespace Multithreading
{
    int _count = 1;
    vector<thread> _workers;

    mutex _mutex;
    condition_variable _cvWork;
    condition_variable _cvDone;

    func _task = 0;
    const void *_arg1 = 0;
    void *_arg2 = 0;

    unsigned long _generation = 0;  // bumped once per exec()
    int _remaining = 0;             // workers yet to finish the current task
    bool _shutdown = false;

    void worker(int id);
}

// Persistent worker. Sleeps until exec() publishes a new task generation,
// runs its slice, then reports completion. This replaces the previous
// design that created and joined a fresh std::thread on every exec() call.
void Multithreading::worker(int id)
{
    unsigned long processed = 0;

    for(;;) {
        func task;
        const void *arg1;
        void *arg2;

        {
            unique_lock<mutex> lock(_mutex);
            _cvWork.wait(lock, [&]{ return _shutdown || _generation != processed; });

            if(_shutdown)
                return;

            processed = _generation;
            task = _task;
            arg1 = _arg1;
            arg2 = _arg2;
        }

        task(id, arg1, arg2);

        {
            unique_lock<mutex> lock(_mutex);
            if(--_remaining == 0)
                _cvDone.notify_one();
        }
    }
}

void Multithreading::init()
{
    const unsigned hardware = thread::hardware_concurrency();
    _count = hardware > 0 ? int(hardware) : 1;  // hardware_concurrency() may return 0

    _shutdown = false;
    _generation = 0;
    _remaining = 0;

    _workers.clear();
    for(int i = 0; i < _count; i++)
        _workers.emplace_back(worker, i);
}

void Multithreading::shutdown()
{
    {
        unique_lock<mutex> lock(_mutex);
        _shutdown = true;
    }
    _cvWork.notify_all();

    for(thread &t : _workers) {
        if(t.joinable())
            t.join();
    }

    _workers.clear();
}

void Multithreading::exec(func task, const void *arg1, void *arg2)
{
    {
        unique_lock<mutex> lock(_mutex);
        _task = task;
        _arg1 = arg1;
        _arg2 = arg2;
        _remaining = _count;
        _generation++;
    }
    _cvWork.notify_all();

    unique_lock<mutex> lock(_mutex);
    _cvDone.wait(lock, []{ return _remaining == 0; });
}

int Multithreading::threads()
{
    return _count;
}
