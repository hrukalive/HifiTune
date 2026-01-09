#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "AnalysisTask.h"
#include "ProgressReporter.h"

class TaskQueue
{
public:
    explicit TaskQueue(size_t workerCount = 1);
    ~TaskQueue();

    void enqueue(std::unique_ptr<AnalysisTask> task, std::shared_ptr<ProgressReporter> reporter);
    void cancelCurrent();
    bool isBusy() const;

private:
    struct TaskItem
    {
        std::unique_ptr<AnalysisTask> task;
        std::shared_ptr<ProgressReporter> reporter;
    };

    void workerLoop();
    void stopWorkers();

    std::vector<std::thread> workers;
    std::queue<TaskItem> tasks;
    mutable std::mutex mutex;
    std::condition_variable condition;
    std::atomic<bool> shuttingDown { false };
    std::atomic<bool> cancelRequested { false };
    std::atomic<bool> runningTask { false };
};
