#include "task_scheduler.h"
#include <iostream>
#include <chrono>

using namespace std;

TaskScheduler::TaskScheduler() : done(false) {}

TaskScheduler::~TaskScheduler() 
{
    stop();
}


void TaskScheduler::addTask(int priority, function<void()> func) 
{
    lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(Task{ priority, func });
    cv.notify_one();
}

void TaskScheduler::start() 
{
    schedulerThread = std::thread(&TaskScheduler::schedulerLoop, this);
}

void TaskScheduler::stop() 
{
    {
        lock_guard<std::mutex> lock(queueMutex);
        done = true;
    }
    cv.notify_one();

    if (schedulerThread.joinable())  schedulerThread.join();
}

void TaskScheduler::schedulerLoop() 
{
    while (true) {
        unique_lock<mutex> lock(queueMutex);

        cv.wait(lock, [this] { return !taskQueue.empty() || done; });

        if (done && taskQueue.empty())  return;

        Task task = taskQueue.top();
        taskQueue.pop();
        lock.unlock();

        task.func();
    }
}

int main() {
    TaskScheduler scheduler;

    scheduler.start();

    scheduler.addTask(1, []() 
        {
        cout << "Task with priority 1 is running" << endl;
        });

    scheduler.addTask(2, []() 
        {
        cout << "Task with priority 2 is running" << endl;
        });

    scheduler.addTask(3, []()
        {
        cout << "Task with priority 3 is running" << endl;
        });

    this_thread::sleep_for(chrono::seconds(2));

    scheduler.stop();
}