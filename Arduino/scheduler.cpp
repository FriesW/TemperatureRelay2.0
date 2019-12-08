#include "scheduler.h"

static void reg(Task * tsk);

void Task::init(
    void (*task_function)(void),
    unsigned long offset,
    unsigned long period,
    char * name
    )
{
    _link = NULL;
    _running = true;
    _fn = task_function;
    _offset = offset;
    _period = period;
    _name = name;
    reg(this);
}

void Task::start()
{
    _running = true;
}

void Task::stop()
{
    _running = false;
}



_Scheduler Scheduler;



static void reg(Task * tsk)
{
    Scheduler._add(tsk);
}
