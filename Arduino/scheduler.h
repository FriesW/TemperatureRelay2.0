#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "system_time.h"

class Task
{
public:

void init(
    void (*task_function)(void),
    unsigned long offset,
    unsigned long period
);

void start();

void stop();

// """private:"""

void (*_fn)(void);
boolean _running;
unsigned long _offset;
unsigned long _period;
unsigned long _next_t;
Task * _link;

};

class _Scheduler
{

public:

_Scheduler()
{

}

void run()
{
    if(current == NULL) while(true) delay(100000);

    running = false;

    Task * start = current;
    do
    {
        running = running || current->_running;
        current->_next_t = current->_offset;
        current = current->_link;
    } while(current != start);

    if(!running) while(true) delay(100000);

    time.reset();

    while(true)
    {
        unsigned long min_time = (unsigned long) -1;
        Task * min_task;
        start = current;
        do
        {
            if( current->_next_t < min_time)
            {
                min_time = current->_next_t;
                min_task = current;
            }
            current = current->_link;
        }
        while(current != start);

        if( min_time > time.now_s() )
            delay( (min_time - time.now_s()) * 1000 - time.now_ms() );
        else
            yield();

        min_task->_fn();

        min_task->_next_t += min_task->_period;

    }
}

void _add(Task * tsk)
{
    if(running)
        tsk->_next_t = time.now_s() + tsk->_offset;
    if(current == NULL)
    {
        tsk->_link = tsk;
        current = tsk;
        return;
    }

    Task * next = current->_link;
    current->_link = tsk;
    tsk->_link = next;

}

private:

boolean running = false;
Task * current = NULL;
SystemTime time;

};

extern _Scheduler Scheduler;

#endif
