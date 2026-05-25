#ifndef MULTITHREADING_H
#define MULTITHREADING_H

namespace Multithreading
{
    void init();
    int threads();

    typedef void(*func)(int, const void*, void*);
    void exec(func task, const void *arg1, void *arg2);
}

#endif // MULTITHREADING_H
