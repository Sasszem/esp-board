#include "tasks.hpp"
#include "Arduino.h"
void TaskManager::add_task(void (*task)(), unsigned int interval, unsigned int t)
{
    Task new_task;
    new_task.interval = interval;
    new_task.task = task;
    new_task.target = t + interval;
    tasks.push_back(new_task);
    task_count++;
}

void TaskManager::update(unsigned int t)
{
    for (int i = 0; i<task_count; i++)
    {
        if (t>=tasks[i].target)
        {
            tasks[i].task();
            tasks[i].target += tasks[i].interval;
        }
    }
}
