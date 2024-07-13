//
// Created by fluty on 2024/1/31.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "Interface/IMainWindow.h"
#include "Modules/Task/TaskManager.h"
#include "Modules/Task/Task.h"

class TaskDialog;
class ProgressIndicator;
class QLabel;
class TracksView;
class ClipEditorView;

class MainWindow final : public QMainWindow, public IMainWindow {
    Q_OBJECT

public:
    explicit MainWindow();
    void setProjectName(const QString &name) override;

public slots:
    void onAllDone();
    void onTaskChanged(TaskManager::TaskChangeType type, Task *task, qsizetype index);
    void onTaskStatusChanged(const TaskStatus &status);

private:
    void closeEvent(QCloseEvent *event) override;

    bool m_isCloseRequested = false;
    bool m_isAllDone = false;

    TracksView *m_tracksView;
    ClipEditorView *m_clipEditView;

    QLabel *m_lbTaskTitle;
    ProgressIndicator *m_progressBar;

    QTimer m_waitDoneDialogDelayTimer;
    TaskDialog *m_waitDoneDialog;

    // int m_noteIndex = 0;
};



#endif // MAINWINDOW_H
