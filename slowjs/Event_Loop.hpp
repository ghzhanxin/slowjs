//
//  Event_Loop.hpp
//  slowjs
//
//  Created by zx on 2021/5/15.
//

#ifndef Event_Loop_hpp
#define Event_Loop_hpp

#include <stdio.h>
#include <queue>

#include "JSValue_Type.hpp"

class Function_Data
{
public:
    Function_Data(
        JSFunction *_fo,
        JSValue _thisValue,
        vector<JSValue> _args)
        : fo(_fo), thisValue(_thisValue), args(_args){};

    JSFunction *fo;
    JSValue thisValue;
    vector<JSValue> args;
};
class Task
{
public:
    Task(Function_Data *data) : fn_data(data){};

    Function_Data *fn_data;
};
class Job : public Task
{
public:
    Job(Function_Data *data) : Task(data){};
};

class Event_Loop
{
public:
    Event_Loop(Slowjs *_slow) : slow(_slow){};
    queue<Task> task_queue;
    queue<Job> job_queue;

    bool hasTask();
    bool hasJob();

    // https://html.spec.whatwg.org/#event-loop-processing-model
    void startLoop();
    void Perform(Task &);
    void PerformOldestTask();
    void PerformJobCheckPoint();

    Slowjs *slow;
};

#endif /* Event_Loop_hpp */
