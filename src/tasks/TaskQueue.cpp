#include "TaskQueue.h"

#include <algorithm>

TaskQueue::TaskQueue(size_t workerCount)
{
    const auto count = std::max<size_t>(1, workerCount);
    workers.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        workers.emplace_back([this] { workerLoop(); });
    }
}

TaskQueue::~TaskQueue()
{
    stopWorkers();
}

void TaskQueue::enqueue(std::unique_ptr<AnalysisTask> task, std::shared_ptr<ProgressReporter> reporter)
{
    if (!task)
        return;

    if (!reporter)
        reporter = std::make_shared<GuiProgressReporter>(nullptr);

    {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push(TaskItem { std::move(task), std::move(reporter) });
    }
    condition.notify_one();
}

void TaskQueue::cancelCurrent()
{
    if (runningTask.load())
        cancelRequested.store(true);
}

bool TaskQueue::isBusy() const
{
    if (runningTask.load())
        return true;

    std::lock_guard<std::mutex> lock(mutex);
    return !tasks.empty();
}

void TaskQueue::workerLoop()
{
    while (true)
    {
        TaskItem item;
        {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this]
            {
                return shuttingDown.load() || !tasks.empty();
            });

            if (shuttingDown.load())
                return;

            item = std::move(tasks.front());
            tasks.pop();
        }

        cancelRequested.store(false);
        runningTask.store(true);

        if (item.reporter)
            item.reporter->reportState(ProgressState::Active, item.task->getName());

        item.task->run(*item.reporter, cancelRequested);

        if (item.reporter)
        {
            if (cancelRequested.load())
                item.reporter->reportState(ProgressState::Cancelled, item.task->getName() + " cancelled");
            else
                item.reporter->reportState(ProgressState::Completed, item.task->getName() + " completed");
        }

        runningTask.store(false);
    }
}

void TaskQueue::stopWorkers()
{
    shuttingDown.store(true);
    condition.notify_all();

    for (auto& worker : workers)
    {
        if (worker.joinable())
            worker.join();
    }
    workers.clear();
}
