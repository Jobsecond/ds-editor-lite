//
// Created by OrangeCat on 24-9-3.
//

#include "InferController.h"
#include "InferController_p.h"

#include "InferControllerHelper.h"
#include "Model/AppModel/InferPiece.h"
#include "Models/PhonemeNameInput.h"
#include "Modules/Audio/AudioContext.h"
#include "Tasks/GetPhonemeNameTask.h"
#include "Tasks/GetPronunciationTask.h"
#include "Utils/Linq.h"
#include "Utils/ValidationUtils.h"

InferController::InferController() : d_ptr(new InferControllerPrivate(this)) {
    Q_D(InferController);
    connect(appStatus, &AppStatus::moduleStatusChanged, d,
            &InferControllerPrivate::onModuleStatusChanged);
    connect(appStatus, &AppStatus::editingChanged, d, &InferControllerPrivate::onEditingChanged);
}

void InferControllerPrivate::onModuleStatusChanged(AppStatus::ModuleType module,
                                                   AppStatus::ModuleStatus status) {
    if (module == AppStatus::ModuleType::Language)
        handleLanguageModuleStatusChanged(status);
}

void InferControllerPrivate::onEditingChanged(AppStatus::EditObjectType type) {
    // TODO：需要处理编辑被取消的情况
    if (type == AppStatus::EditObjectType::Note) {
        qWarning() << "正在编辑工程，取消相关任务";
        auto clip = appModel->findClipById(appStatus->activeClipId);
        if (clip->clipType() == IClip::Singing)
            cancelClipRelatedTasks(reinterpret_cast<SingingClip *>(clip));
    } else if (type == AppStatus::EditObjectType::None) {
        // if (m_lastEditObjectType == AppStatus::EditObjectType::Note) {
        //     qInfo() << "编辑完成，重新创建任务";
        //     auto clip = appModel->findClipById(appStatus->activeClipId);
        //     if (clip->clipType() == IClip::Singing)
        //         createAndRunGetPronTask(dynamic_cast<SingingClip *>(clip));
        // }
    }
    m_lastEditObjectType = type;
}

void InferControllerPrivate::handleTempoChanged(double tempo) {
    reset();
    recreateAllInferTasks();
}

void InferControllerPrivate::handleSingingClipInserted(SingingClip *clip) {
    ModelChangeHandler::handleSingingClipInserted(clip);
    clip->reSegment();
    createAndRunGetPronTask(*clip);
}

void InferControllerPrivate::handleSingingClipRemoved(SingingClip *clip) {
    ModelChangeHandler::handleSingingClipRemoved(clip);
    cancelClipRelatedTasks(clip);
}

void InferControllerPrivate::handleNoteChanged(SingingClip::NoteChangeType type,
                                               const QList<Note *> &notes, SingingClip *clip) {
    switch (type) {
        case SingingClip::Insert:
        case SingingClip::Remove:
        case SingingClip::EditedWordPropertyChange:
        case SingingClip::EditedPhonemeOffsetChange:
        case SingingClip::TimeKeyPropertyChange:
            // 音符发生改动，其所属分段必定需要重新推理。将该分段标记为脏，以便在分段前丢弃
            for (const auto &piece : clip->findPiecesByNotes(notes)) {
                piece->dirty = true;
                cancelPieceRelatedTasks(piece->id());
            }
            clip->reSegment();
            createAndRunGetPronTask(*clip);
            break;
        default:
            break;
    } // Ignore original word property change
}

void InferControllerPrivate::handleParamChanged(ParamInfo::Name name, Param::Type type,
                                                SingingClip *clip) {
    if (type != Param::Edited)
        return;
    auto dirtyPieces = InferControllerHelper::findDirtyParamPieces(name, *clip);
    switch (name) {
        case ParamInfo::Expressiveness:
            break;
        case ParamInfo::Pitch:
            for (const auto &piece : dirtyPieces) {
                auto pred = L_PRED(t, t->pieceId() == piece->id());
                m_inferVarianceTasks.cancelIf(pred);
                m_inferAcousticTasks.cancelIf(pred);
                createAndRunInferVarianceTask(*piece);
                piece->acousticInferStatus = Running;
            }
            break;
        case ParamInfo::Energy:
        case ParamInfo::Breathiness:
        case ParamInfo::Voicing:
        case ParamInfo::Tension:
            break;
        case ParamInfo::Gender:
        case ParamInfo::Velocity:
        case ParamInfo::Unknown:
            break;
    }
}

void InferControllerPrivate::handleLanguageModuleStatusChanged(AppStatus::ModuleStatus status) {
    if (status == AppStatus::ModuleStatus::Ready) {
        qDebug() << "语言模块就绪，开始运行任务";
        runNextGetPronTask();
    } else if (status == AppStatus::ModuleStatus::Error) {
        m_getPronTasks.disposePendingTasks();
        qCritical() << "未能启动语言模块，已取消任务";
    }
}

void InferControllerPrivate::handleGetPronTaskFinished(GetPronunciationTask &task) {
    m_getPronTasks.onCurrentFinished();
    runNextGetPronTask();
    auto clip = appModel->findClipById(task.clipId());
    if (task.terminated() || !clip) {
        delete &task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(clip);
    InferControllerHelper::updatePronunciation(task.notesRef, task.result, *singingClip);
    createAndRunGetPhoneTask(*singingClip);
    delete &task;
}

void InferControllerPrivate::handleGetPhoneTaskFinished(GetPhonemeNameTask &task) {
    m_getPhoneTasks.onCurrentFinished();
    runNextGetPhoneTask();
    auto clip = appModel->findClipById(task.clipId());
    if (task.terminated() || !clip) {
        delete &task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(clip);
    InferControllerHelper::updatePhoneName(task.notesRef, task.result, *singingClip);
    if (ValidationUtils::canInferDuration(*singingClip))
        for (const auto piece : singingClip->pieces())
            createAndRunInferDurTask(*piece);
    else
        qWarning() << "音素序列有错误，无法创建时长推理任务 clipId:" << clip->id();
    delete &task;
}

void InferControllerPrivate::handleInferDurTaskFinished(InferDurationTask &task) {
    m_inferDurTasks.onCurrentFinished();
    runNextInferDurTask();
    auto clip = appModel->findClipById(task.clipId());
    if (task.terminated() || !task.success() || !clip) {
        delete &task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(appModel->findClipById(task.clipId()));
    auto piece = singingClip->findPieceById(task.pieceId());
    auto modelNoteCount = piece->notes.count();
    auto taskNoteCount = task.result().count();
    if (modelNoteCount != taskNoteCount) {
        // piece->acousticInferStatus = Failed;
        qFatal() << "模型音符数不等于任务音符数"
                 << "模型音符数：" << modelNoteCount << "任务音符数：" << taskNoteCount;
        return;
    }
    // 推理成功，保存本次推理的输入以便之后比较
    m_lastInferDurInputs[task.pieceId()] = task.input();
    InferControllerHelper::updatePhoneOffset(piece->notes, task.result(), *singingClip);
    createAndRunInferPitchTask(*piece);
    // piece->acousticInferStatus = Success;
    delete &task;
}

void InferControllerPrivate::handleInferPitchTaskFinished(InferPitchTask &task) {
    m_inferPitchTasks.onCurrentFinished();
    runNextInferPitchTask();
    auto clip = appModel->findClipById(task.clipId());
    if (task.terminated() || !clip) {
        delete &task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(appModel->findClipById(task.clipId()));
    auto piece = singingClip->findPieceById(task.pieceId());
    if (!piece || !task.success()) {
        delete &task;
        return;
    }
    if (task.success()) {
        // 推理成功，保存本次推理的输入以便之后比较
        m_lastInferPitchInputs[task.pieceId()] = task.input();
        InferControllerHelper::updatePitch(task.result(), *piece);
        createAndRunInferVarianceTask(*piece);
    } else
        piece->acousticInferStatus = Failed;
    delete &task;
}

void InferControllerPrivate::handleInferVarianceTaskFinished(InferVarianceTask &task) {
    m_inferVarianceTasks.onCurrentFinished();
    runNextInferVarianceTask();
    auto clip = appModel->findClipById(task.clipId());
    if (task.terminated() || !clip) {
        delete &task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(appModel->findClipById(task.clipId()));
    auto piece = singingClip->findPieceById(task.pieceId());
    if (!piece || !task.success()) {
        delete &task;
        return;
    }
    if (task.success()) {
        m_lastInferVarianceInputs[task.pieceId()] = task.input();
        InferControllerHelper::updateVariance(task.result(), *piece);
        createAndRunInferAcousticTask(*piece);
    } else {
        piece->acousticInferStatus = Failed;
    }
    delete &task;
}

void InferControllerPrivate::handleInferAcousticTaskFinished(InferAcousticTask &task) {
    m_inferAcousticTasks.onCurrentFinished();
    runNextInferAcousticTask();
    auto clip = appModel->findClipById(task.clipId());
    if (task.terminated() || !clip) {
        delete &task;
        return;
    }

    auto singingClip = dynamic_cast<SingingClip *>(appModel->findClipById(task.clipId()));
    auto piece = singingClip->findPieceById(task.pieceId());
    if (!piece) {
        delete &task;
        return;
    }
    if (task.success()) {
        m_lastInferAcousticInputs[task.pieceId()] = task.input();
        InferControllerHelper::updateAcoustic(task.result(), *piece);
    } else
        piece->acousticInferStatus = Failed;
    delete &task;
}

void InferControllerPrivate::recreateAllInferTasks() {
    for (const auto &track : appModel->tracks())
        for (const auto &clip : track->clips()) {
            if (clip->clipType() != IClip::Singing)
                continue;
            auto singingClip = reinterpret_cast<SingingClip *>(clip);
            for (const auto &piece : singingClip->pieces())
                piece->dirty = true;
            singingClip->reSegment();
            for (const auto &piece : singingClip->pieces()) {
                InferControllerHelper::resetPhoneOffset(piece->notes, *piece);
                createAndRunInferDurTask(*piece);
            }
        }
}

void InferControllerPrivate::createAndRunGetPronTask(SingingClip &clip) {
    auto task = new GetPronunciationTask(clip.id(), clip.notes().toList());
    connect(task, &Task::finished, this, [=] { handleGetPronTaskFinished(*task); });
    m_getPronTasks.add(task);
    if (!m_getPronTasks.current)
        runNextGetPronTask();
}

void InferControllerPrivate::createAndRunGetPhoneTask(SingingClip &clip) {
    QList<PhonemeNameInput> inputs;
    for (const auto note : clip.notes())
        inputs.append({note->lyric(), note->pronunciation().result()});
    auto task = new GetPhonemeNameTask(clip.id(), inputs);
    task->notesRef = clip.notes().toList();
    connect(task, &Task::finished, this, [=] { handleGetPhoneTaskFinished(*task); });
    m_getPhoneTasks.add(task);
    if (!m_getPhoneTasks.current)
        runNextGetPhoneTask();
}

void InferControllerPrivate::createAndRunInferDurTask(InferPiece &piece) {
    const auto inputNotes = InferControllerHelper::buildInferInputNotes(piece.notes);
    const InferDurationTask::InferDurInput input = {piece.clip->id(), piece.id(), inputNotes,
                                                    m_singerConfigPath, appModel->tempo()};
    // 创建分段的推理任务前，首先检查输入是否和上次的相同。如果相同，则直接忽略，避免不必要的推理
    if (m_lastInferDurInputs.contains(piece.id())) {
        return;
        // TODO: 存在比较结果错误的问题，暂时取消比较具体内容
        // if (const auto lastInput = m_lastInferDurInputs[piece.id()]; lastInput == input)
        //     return;
    }
    // 清空原有的自动参数
    InferControllerHelper::resetPhoneOffset(piece.notes, piece);
    auto task = new InferDurationTask(input);
    connect(task, &Task::finished, this, [=] { handleInferDurTaskFinished(*task); });
    m_inferDurTasks.add(task);
    piece.acousticInferStatus = Running;
    if (!m_inferDurTasks.current)
        runNextInferDurTask();
}

void InferControllerPrivate::createAndRunInferPitchTask(InferPiece &piece) {
    const auto inputNotes = InferControllerHelper::buildInferInputNotes(piece.notes);
    const InferPitchTask::InferPitchInput input = {piece.clipId(), piece.id(), inputNotes,
                                                   m_singerConfigPath, appModel->tempo()};
    if (m_lastInferPitchInputs.contains(piece.id()))
        if (const auto lastInput = m_lastInferPitchInputs[piece.id()]; lastInput == input)
            return;
    InferControllerHelper::resetPitch(piece);
    auto task = new InferPitchTask(input);
    connect(task, &Task::finished, this, [=] { handleInferPitchTaskFinished(*task); });
    m_inferPitchTasks.add(task);
    if (!m_inferPitchTasks.current)
        runNextInferPitchTask();
}

void InferControllerPrivate::createAndRunInferVarianceTask(InferPiece &piece) {
    const auto input = InferControllerHelper::buildInferVarianceInput(piece, m_singerConfigPath);
    if (m_lastInferVarianceInputs.contains(piece.id()))
        if (const auto lastInput = m_lastInferVarianceInputs[piece.id()]; lastInput == input)
            return;
    InferControllerHelper::resetVariance(piece);
    auto task = new InferVarianceTask(input);
    connect(task, &Task::finished, this, [=] { handleInferVarianceTaskFinished(*task); });
    m_inferVarianceTasks.add(task);
    if (!m_inferVarianceTasks.current)
        runNextInferVarianceTask();
}

void InferControllerPrivate::createAndRunInferAcousticTask(InferPiece &piece) {
    const auto inputNotes = InferControllerHelper::buildInferInputNotes(piece.notes);
    InferParamCurve pitch;
    InferParamCurve breathiness;
    InferParamCurve tension;
    InferParamCurve voicing;
    InferParamCurve energy;
    InferParamCurve gender;
    InferParamCurve velocity;
    for (const auto &value : piece.inputPitch.values())
        pitch.values.append(value / 100.0);

    for (const auto &value : piece.originalBreathiness.values())
        breathiness.values.append(value / 1000.0);
    for (const auto &value : piece.originalTension.values())
        tension.values.append(value / 1000.0);
    for (const auto &value : piece.originalVoicing.values())
        voicing.values.append(value / 1000.0);
    for (const auto &value : piece.originalEnergy.values()) {
        energy.values.append(value / 1000.0);

        gender.values.append(0);
        velocity.values.append(1);
    }

    const InferAcousticTask::InferAcousticInput input = {
        piece.clipId(),    piece.id(), inputNotes,  m_singerConfigPath,
        appModel->tempo(), pitch,      breathiness, tension,
        voicing,           energy,     gender,      velocity};
    if (m_lastInferAcousticInputs.contains(piece.id()))
        if (const auto lastInput = m_lastInferAcousticInputs[piece.id()]; lastInput == input)
            return;
    // InferControllerHelper::resetVariance(piece);
    auto task = new InferAcousticTask(input);
    connect(task, &Task::finished, this, [=] { handleInferAcousticTaskFinished(*task); });
    m_inferAcousticTasks.add(task);
    if (!m_inferAcousticTasks.current)
        runNextInferAcousticTask();
}

void InferControllerPrivate::reset() {
    m_getPronTasks.cancelAll();
    m_getPhoneTasks.cancelAll();
    m_inferDurTasks.cancelAll();
    m_inferPitchTasks.cancelAll();
    m_inferVarianceTasks.cancelAll();
    m_inferAcousticTasks.cancelAll();

    m_lastInferDurInputs.clear();
    m_lastInferPitchInputs.clear();
    m_lastInferVarianceInputs.clear();
    m_lastInferAcousticInputs.clear();
}

void InferControllerPrivate::cancelClipRelatedTasks(SingingClip *clip) {
    qInfo() << "取消歌声剪辑相关任务"
            << "clipId:" << clip->id();
    auto pred = L_PRED(t, t->clipId() == clip->id());
    m_getPronTasks.cancelIf(pred);
    m_getPhoneTasks.cancelIf(pred);
    for (const auto piece : clip->pieces())
        cancelPieceRelatedTasks(piece->id());
}

void InferControllerPrivate::cancelPieceRelatedTasks(int pieceId) {
    qInfo() << "取消分段相关任务"
            << "pieceId:" << pieceId;
    auto pred = L_PRED(t, t->pieceId() == pieceId);
    m_inferDurTasks.cancelIf(pred);
    m_inferPitchTasks.cancelIf(pred);
    m_inferVarianceTasks.cancelIf(pred);
    m_inferAcousticTasks.cancelIf(pred);
}

void InferControllerPrivate::runNextGetPronTask() {
    if (appStatus->languageModuleStatus == AppStatus::ModuleStatus::Ready)
        m_getPronTasks.runNext();
}

void InferControllerPrivate::runNextGetPhoneTask() {
    if (appStatus->languageModuleStatus == AppStatus::ModuleStatus::Ready)
        m_getPhoneTasks.runNext();
}

void InferControllerPrivate::runNextInferDurTask() {
    m_inferDurTasks.runNext();
}

void InferControllerPrivate::runNextInferPitchTask(){
    m_inferPitchTasks.runNext();
}

void InferControllerPrivate::runNextInferVarianceTask(){
    m_inferVarianceTasks.runNext();
}

void InferControllerPrivate::runNextInferAcousticTask(){
    m_inferAcousticTasks.runNext();
}