//
//  Event_Loop.cpp
//  slowjs
//
//  Created by zx on 2021/5/15.
//

#include "Event_Loop.hpp"
#include "Abstract_Operation.hpp"

void Event_Loop::startLoop()
{
    while (1)
    {
        PerformOldestTask();
        PerformJobCheckPoint();

        if (!hasTask() && !hasJob())
            break;
    }
}
bool Event_Loop::hasTask()
{
    return !!task_queue.size();
}
bool Event_Loop::hasJob()
{
    return !!job_queue.size();
}
void Event_Loop::Perform(Task *task)
{
    task->fn_data->fo->Call(slow, task->fn_data->thisValue, task->fn_data->args);
}
void Event_Loop::PerformOldestTask()
{
    if (hasTask())
    {
        Task *task = task_queue.front();
        task_queue.pop();
        Perform(task);
    }
}
void Event_Loop::PerformJobCheckPoint()
{
    while (hasJob())
    {
        Job *job = job_queue.front();
        job_queue.pop();
        Perform(job);
    }
}
