//
// Created by OrangeCat on 24-9-3.
//

#include "InferController.h"
#include "InferController_p.h"

#include "Actions/AppModel/Note/NoteActions.h"
#include "Model/Inference/InferDurNote.h"
#include "Model/Inference/InferPiece.h"
#include "Modules/Inference/InferDurationTask.h"
#include "Modules/Task/TaskManager.h"
#include "Tasks/GetPhonemeNameTask.h"
#include "Tasks/GetPronunciationTask.h"
#include "Utils/Linq.h"
#include "Utils/NoteWordUtils.h"
#include "Utils/OriginalParamUtils.h"

InferController::InferController() : d_ptr(new InferControllerPrivate(this)) {
    Q_D(InferController);
    connect(appModel, &AppModel::modelChanged, d, &InferControllerPrivate::onModelChanged);
    connect(appStatus, &AppStatus::moduleStatusChanged, d,
            &InferControllerPrivate::onModuleStatusChanged);
    connect(appStatus, &AppStatus::editingChanged, d, &InferControllerPrivate::onEditingChanged);
}

void InferControllerPrivate::onModelChanged() {
    for (const auto track : m_tracks)
        onTrackChanged(AppModel::Remove, -1, track);

    for (const auto track : appModel->tracks())
        onTrackChanged(AppModel::Insert, -1, track);
}

void InferControllerPrivate::onTrackChanged(AppModel::TrackChangeType type, qsizetype index,
                                     Track *track) {
    if (type == AppModel::Insert) {
        // qDebug() << "onTrackChanged" << "Insert";
        m_tracks.append(track);
        for (const auto clip : track->clips())
            handleClipInserted(clip);
        connect(track, &Track::clipChanged, this, &InferControllerPrivate::onClipChanged);
    } else if (type == AppModel::Remove) {
        // qDebug() << "onTrackChanged" << "Remove";
        m_tracks.removeOne(track);
        for (const auto clip : track->clips())
            handleClipRemoved(clip);
        disconnect(track, &Track::clipChanged, this, &InferControllerPrivate::onClipChanged);
    }
}

void InferControllerPrivate::onClipChanged(Track::ClipChangeType type, Clip *clip) {
    if (type == Track::Inserted) {
        // qDebug() << "onClipChanged" << "Inserted";
        handleClipInserted(clip);
    } else if (type == Track::Removed) {
        // qDebug() << "onClipChanged" << "Removed";
        handleClipRemoved(clip);
    }
}

void InferControllerPrivate::onModuleStatusChanged(AppStatus::ModuleType module,
                                            AppStatus::ModuleStatus status) {
    if (module == AppStatus::ModuleType::Language) {
        handleLanguageModuleStatusChanged(status);
    }
}

void InferControllerPrivate::onEditingChanged(AppStatus::EditObjectType type) {
    if (type == AppStatus::EditObjectType::Note) {
        qWarning() << "正在编辑工程，取消相关任务";
        auto clip = appModel->findClipById(appStatus->activeClipId);
        cancelClipRelatedTasks(clip);
    } else if (type == AppStatus::EditObjectType::None) {
        // TODO: 按需重建相关任务
        // if (m_lastEditObjectType == AppStatus::EditObjectType::Note) {
        // qInfo() << "编辑完成，重新创建任务";
        // auto clip = appModel->findClipById(appStatus->activeClipId);
        // if (clip->clipType() == IClip::Singing)
        //     createAndRunGetPronTask(dynamic_cast<SingingClip *>(clip));
        // }
    }
    m_lastEditObjectType = type;
}

void InferControllerPrivate::handleClipInserted(Clip *clip) {
    // m_clips.append(clip);
    if (clip->clipType() == Clip::Singing) {
        auto singingClip = reinterpret_cast<SingingClip *>(clip);
        connect(singingClip, &SingingClip::noteChanged, this,
                [=](SingingClip::NoteChangeType type, const QList<Note *> &notes) {
                    handleNoteChanged(type, notes, singingClip);
                });
        singingClip->reSegment();
        createAndRunGetPronTask(singingClip);
    }
}

void InferControllerPrivate::handleClipRemoved(Clip *clip) {
    qDebug() << "handleClipRemoved" << clip->id();
    // m_clips.removeOne(clip);
    cancelClipRelatedTasks(clip);
    disconnect(clip, nullptr, this, nullptr);
}

void InferControllerPrivate::handleNoteChanged(SingingClip::NoteChangeType type,
                                        const QList<Note *> &notes, SingingClip *clip) {
    switch (type) {
        case SingingClip::Insert:
        case SingingClip::Remove:
        case SingingClip::EditedWordPropertyChange:
        case SingingClip::TimeKeyPropertyChange:
            clip->reSegment();
            cancelClipRelatedTasks(clip);
            createAndRunGetPronTask(clip);
            break;
        default:
            break;
    } // Ignore original word property change
}

void InferControllerPrivate::handleLanguageModuleStatusChanged(AppStatus::ModuleStatus status) {
    if (status == AppStatus::ModuleStatus::Ready) {
        qDebug() << "语言模块就绪，开始运行任务";
        runNextGetPronTask();
    } else if (status == AppStatus::ModuleStatus::Error) {
        for (const auto task : m_getPronTaskQueue) {
            taskManager->removeTask(task);
            disconnect(task, nullptr, this, nullptr);
            delete task;
        }
        qCritical() << "未能启动语言模块，已取消任务";
    }
}

void InferControllerPrivate::handleGetPronTaskFinished(GetPronunciationTask *task) {
    auto terminate = task->terminated();
    qInfo() << "获取发音任务完成 clipId:" << task->clipId() << "taskId:" << task->id()
            << "terminate:" << terminate;
    disconnect(task, nullptr, this, nullptr);
    taskManager->removeTask(task);
    m_runningGetPronTask = nullptr;
    runNextGetPronTask();

    auto clip = appModel->findClipById(task->clipId());
    if (terminate || !clip) {
        delete task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(clip);
    OriginalParamUtils::updateNotesPronunciation(task->notesRef, task->result, singingClip);
    createAndRunGetPhonemeNameTask(singingClip);
    delete task;
}

void InferControllerPrivate::handleGetPhonemeNameTaskFinished(GetPhonemeNameTask *task) {
    auto terminate = task->terminated();
    qInfo() << "获取音素名称任务完成 clipId:" << task->clipId() << "taskId:" << task->id()
            << "terminate:" << terminate;
    disconnect(task, nullptr, this, nullptr);
    taskManager->removeTask(task);
    m_runningGetPhonemeNameTask = nullptr;
    runNextGetPhonemeNameTask();

    auto clip = appModel->findClipById(task->clipId());
    if (terminate || !clip) {
        delete task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(clip);
    OriginalParamUtils::updateNotesPhonemeName(task->notesRef, task->result, singingClip);
    createAndRunInferDurTask(singingClip);
    delete task;
}

void InferControllerPrivate::handleInferDurTaskFinished(InferDurationTask *task) {
    auto terminate = task->terminated();
    qInfo() << "时长推理任务完成 clipId:" << task->clipId() << "taskId:" << task->id()
            << "terminate:" << terminate;
    disconnect(task, nullptr, this, nullptr);
    taskManager->removeTask(task);
    m_runningInferDurTask = nullptr;
    runNextInferDurTask();

    auto clip = appModel->findClipById(task->clipId());
    if (terminate || !clip) {
        delete task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(appModel->findClipById(task->clipId()));
    auto piece = singingClip->findPieceById(task->pieceId());
    auto modelNoteCount = piece->notes.count();
    auto taskNoteCount = task->result().count();
    qDebug() << "模型音符数：" << modelNoteCount << "任务音符数：" << taskNoteCount;
    if (modelNoteCount != taskNoteCount) {
        qFatal() << "模型音符数不等于任务音符数";
        return;
    }
    OriginalParamUtils::updateNotesPhonemeOffset(piece->notes, task->result(), singingClip);
    delete task;
}

bool InferControllerPrivate::validateForInferDuration(int clipId) {
    auto clip = dynamic_cast<SingingClip *>(appModel->findClipById(clipId));
    if (!clip) {
        qCritical() << "Invalid clip type";
        return false;
    }
    if (clip->notes().hasOverlappedItem())
        return false;
    for (const auto note : clip->notes()) {
        if (note->pronunciation().result().isEmpty()) {
            qCritical() << "Invalid note pronunciation";
            return false;
        }
        // TODO: 校验音素名称序列
        // if (note->phonemeNameInfo().isEmpty()) {
        //     qCritical() << "Invalid note phonemeNameInfo" << "note" << note->lyric() <<
        //     note->pronunciation().result(); return false;
        // }
    }
    return true;
}

void InferControllerPrivate::createAndRunGetPronTask(SingingClip *clip) {
    auto task = new GetPronunciationTask(clip->id(), clip->notes().toList());
    qInfo() << "创建获取发音任务 clipId:" << clip->id() << "taskId:" << task->id();
    connect(task, &Task::finished, this, [=] { handleGetPronTaskFinished(task); });
    taskManager->addTask(task);
    m_getPronTaskQueue.enqueue(task);
    if (!m_runningGetPronTask)
        runNextGetPronTask();
}

void InferControllerPrivate::createAndRunGetPhonemeNameTask(SingingClip *clip) {
    QList<PhonemeNameInput> inputs;
    for (const auto note : clip->notes())
        inputs.append({note->lyric(), note->pronunciation().result()});
    auto task = new GetPhonemeNameTask(clip->id(), inputs);
    task->notesRef = clip->notes().toList();
    qInfo() << "创建获取音素名称任务 clipId:" << clip->id() << "taskId:" << task->id()
            << "noteCount:" << clip->notes().count();
    connect(task, &Task::finished, this, [=] { handleGetPhonemeNameTaskFinished(task); });
    taskManager->addTask(task);
    m_getPhonemeNameTaskQueue.enqueue(task);
    if (!m_runningGetPhonemeNameTask)
        runNextGetPhonemeNameTask();
}

void InferControllerPrivate::createAndRunInferDurTask(SingingClip *clip) {
    auto inferDur = [=](const InferPiece &piece) {
        QList<InferDurNote> inputNotes;
        for (const auto note : piece.notes) {
            inputNotes.append(InferDurNote(*note));
        }
        auto durTask = new InferDurationTask(
            {clip->id(), piece.id(), inputNotes,
             R"(F:\Sound libraries\DiffSinger\OpenUtau\Singers\Junninghua_v1.4.0_DiffSinger_OpenUtau\dsconfig.yaml)",
             appModel->tempo()});
        qDebug() << "音素序列校验通过，创建时长推理任务 clipId:" << clip->id()
                 << "pieceId:" << piece.id() << "taskId:" << durTask->id();
        connect(durTask, &Task::finished, this, [=] { handleInferDurTaskFinished(durTask); });
        taskManager->addTask(durTask);
        m_inferDurTaskQueue.enqueue(durTask);
        if (!m_runningInferDurTask)
            runNextInferDurTask();
    };

    if (validateForInferDuration(clip->id())) {
        for (const auto piece : clip->pieces()) {
            inferDur(*piece);
        }
    } else
        qWarning() << "音素序列有错误，无法创建时长推理任务 clipId:" << clip->id();
}

void InferControllerPrivate::cancelClipRelatedTasks(Clip *clip) {
    qInfo() << "取消歌声剪辑相关任务";
    auto pred = [=](auto t) { return t->clipId() == clip->id(); };

    // Cancel get pron tasks
    for (const auto task : Linq::where(m_getPronTaskQueue, pred)) {
        disconnect(task, nullptr, this, nullptr);
        m_getPronTaskQueue.remove(task);
        taskManager->removeTask(task);
        qDebug() << "移除待运行的获取发音任务 clipId:" << task->clipId() << "taskId:" << task->id();
        delete task;
    }
    if (m_runningGetPronTask && pred(m_runningGetPronTask)) {
        taskManager->terminateTask(m_runningGetPronTask);
        qDebug() << "终止正在运行的获取发音任务 clipId:" << m_runningGetPronTask->clipId()
                 << "taskId:" << m_runningGetPronTask->id();
    }

    // 取消获取音素名称任务
    for (const auto task : Linq::where(m_getPhonemeNameTaskQueue, pred)) {
        disconnect(task, nullptr, this, nullptr);
        m_getPhonemeNameTaskQueue.remove(task);
        taskManager->removeTask(task);
        qDebug() << "移除待运行的获取音素名称任务 clipId:" << task->clipId()
                 << "taskId:" << task->id();
        delete task;
    }
    if (m_runningGetPhonemeNameTask && pred(m_runningGetPhonemeNameTask)) {
        taskManager->terminateTask(m_runningGetPhonemeNameTask);
        qDebug() << "终止正在运行的获取音素名称任务 clipId:"
                 << m_runningGetPhonemeNameTask->clipId()
                 << "taskId:" << m_runningGetPhonemeNameTask->id();
    }

    // Cancel infer dur tasks
    for (const auto task : Linq::where(m_inferDurTaskQueue, pred)) {
        disconnect(task, nullptr, this, nullptr);
        m_inferDurTaskQueue.remove(task);
        taskManager->removeTask(task);
        qDebug() << "移除待运行的时长推理任务 clipId:" << task->clipId() << "taskId:" << task->id();
        delete task;
    }
    if (m_runningInferDurTask && pred(m_runningInferDurTask)) {
        taskManager->terminateTask(m_runningInferDurTask);
        qDebug() << "终止正在运行的时长推理任务 clipId:" << m_runningInferDurTask->clipId()
                 << "taskId:" << m_runningInferDurTask->id();
    }
}

void InferControllerPrivate::runNextGetPronTask() {
    if (appStatus->languageModuleStatus != AppStatus::ModuleStatus::Ready)
        return;
    if (m_getPronTaskQueue.count() <= 0)
        return;

    const auto task = m_getPronTaskQueue.dequeue();
    qDebug() << "运行获取发音任务 clipId:" << task->clipId() << "taskId:" << task->id();
    taskManager->startTask(task);
    m_runningGetPronTask = task;
}

void InferControllerPrivate::runNextGetPhonemeNameTask() {
    if (appStatus->languageModuleStatus != AppStatus::ModuleStatus::Ready)
        return;
    if (m_getPhonemeNameTaskQueue.count() <= 0)
        return;

    const auto task = m_getPhonemeNameTaskQueue.dequeue();
    qDebug() << "运行获取音素任务 clipId:" << task->clipId() << "taskId:" << task->id();
    taskManager->startTask(task);
    m_runningGetPhonemeNameTask = task;
}

void InferControllerPrivate::runNextInferDurTask() {
    if (m_inferDurTaskQueue.count() <= 0)
        return;

    const auto task = m_inferDurTaskQueue.dequeue();
    qDebug() << "运行时长推理任务 clipId:" << task->clipId() << "taskId:" << task->id();
    taskManager->startTask(task);
    m_runningInferDurTask = task;
}