#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

struct Task 
{
    int priority;                           
    std::function<void()> func;

    bool operator<(const Task& other) const
    {
        return priority < other.priority;
    }
};

class TaskScheduler 
{
public:
   
    TaskScheduler();

    ~TaskScheduler();

    void addTask(int priority, std::function<void()> func);

    void start();

    
    void stop();

private:
    
    void schedulerLoop();

    std::priority_queue<Task> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool done;
    std::thread schedulerThread;
};