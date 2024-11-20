//
// Created by fluty on 24-2-26.
//

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QRunnable>
#include <QMutex>

#include "Global/TaskGlobal.h"
#include "Utils/UniqueObject.h"

class TaskStatus {
public:
    int id = -1;
    int progress = 0;
    int maximum = 100;
    int minimum = 0;
    bool isIndetermine = false;
    TaskGlobal::Status runningStatus = TaskGlobal::Normal;
    QString title = "Task";
    QString message = "Message";
};

class Task : public QObject, public QRunnable, public UniqueObject {
    Q_OBJECT
public:
    explicit Task(QObject *parent = nullptr) : QObject(parent) {
        setAutoDelete(false);
    }

    explicit Task(int id, QObject *parent = nullptr) : QObject(parent), UniqueObject(id) {
        setAutoDelete(false);
    }

    // ~Task() override {
    //     terminate();
    // };

    virtual void terminate();

    [[nodiscard]] bool started() {
        QMutexLocker locker(&m_mutex);
        return m_started;
    }

    bool stopped() {
        QMutexLocker locker(&m_mutex);
        return m_stopped;
    }

    bool terminated() {
        QMutexLocker locker(&m_mutex);
        return m_abortFlag;
    }

    int priority() const {
        return m_priority;
    }

    void setPriority(const int priority) {
        m_priority = priority;
    }

    [[nodiscard]] const TaskStatus &status() const;
    void setStatus(const TaskStatus &status);

signals:
    void statusUpdated(const TaskStatus &status);
    void finished();

protected:
    virtual void runTask() = 0;
    bool isTerminateRequested();

    int m_priority = 0;

private:
    friend class TaskManager;

    void run() override {
        if (!m_started) {
            m_started = true;
            runTask();
            emit finished();
            m_stopped = true;
        }
    }

    TaskStatus m_status;
    QMutex m_mutex;
    bool m_started = false;
    bool m_abortFlag = false;
    bool m_stopped = false;
};

inline const TaskStatus &Task::status() const {
    return m_status;
}

inline void Task::setStatus(const TaskStatus &status) {
    m_status = status;
    emit statusUpdated(status);
}

inline void Task::terminate() {
    QMutexLocker locker(&m_mutex);
    m_abortFlag = true;
}

inline bool Task::isTerminateRequested() {
    QMutexLocker locker(&m_mutex);
    return m_abortFlag;
}

#endif // TASK_H
