//
// Created by fluty on 24-2-26.
//

#include "TaskManager.h"

#include <QtConcurrent/QtConcurrent>

#include "ITask.h"

void BackgroundWorker::terminateTask(ITask *task) {
    task->terminate();
}
void BackgroundWorker::wait() {
    auto threadPool = QThreadPool::globalInstance();
    threadPool->waitForDone();
    emit waitDone();
}
TaskManager::TaskManager(QObject *parent) : QObject(parent) {
    connect(&m_worker, &BackgroundWorker::waitDone, this, &TaskManager::onWorkerWaitDone);
    m_worker.moveToThread(&m_thread);
}
TaskManager::~TaskManager() {
    terminateAllTasks();
}
const QList<ITask *> &TaskManager::tasks() const {
    return m_tasks;
}
ITask *TaskManager::findTaskById(int id) {
    for (const auto task : m_tasks)
        if (task->id() == id)
            return task;

    return nullptr;
}
void TaskManager::wait() {
    m_worker.wait();
    // threadPool->waitForDone();
}
void TaskManager::addTask(ITask *task) {
    qDebug() << "TaskManager::addTask" << task->id();
    auto index = m_tasks.count();
    m_tasks.append(task);
    emit taskChanged(Added, task, index);
}
void TaskManager::startTask(ITask *task) {
    qDebug() << "TaskManager::startTask";
    threadPool->start(task);
}
void TaskManager::removeTask(ITask *task) {
    auto index = m_tasks.indexOf(task);
    m_tasks.removeOne(task);
    emit taskChanged(Removed, task, index);
}
void TaskManager::startAllTasks() {
    for (const auto &task : m_tasks)
        threadPool->start(task);
}
void TaskManager::terminateTask(ITask *task) {
    BackgroundWorker::terminateTask(task);
}
void TaskManager::terminateAllTasks() {
    for (const auto &task : m_tasks)
        BackgroundWorker::terminateTask(task);
}
void TaskManager::onWorkerWaitDone() {
    emit allDone();
}