//
// Created by fluty on 24-9-26.
//

#ifndef INFERCONTROLLERPRIVATE_H
#define INFERCONTROLLERPRIVATE_H

#include "ModelChangeHandler.h"
#include "Model/AppModel/SingingClip.h"
#include "Model/AppStatus/AppStatus.h"
#include "Modules/Inference/InferDurationTask.h"
#include "Modules/Inference/InferPitchTask.h"
#include "Modules/Task/TaskQueue.h"

class GetPronunciationTask;
class GetPhonemeNameTask;
class InferController;

class InferControllerPrivate : public ModelChangeHandler {
    Q_OBJECT
    Q_DECLARE_PUBLIC(InferController)

public:
    explicit InferControllerPrivate(InferController *q) : ModelChangeHandler(q), q_ptr(q){};

public slots:
    void onModuleStatusChanged(AppStatus::ModuleType module, AppStatus::ModuleStatus status);
    void onEditingChanged(AppStatus::EditObjectType type);

public:
    void handleSingingClipInserted(SingingClip *clip) override;
    void handleSingingClipRemoved(SingingClip *clip) override;
    void handleNoteChanged(SingingClip::NoteChangeType type, const QList<Note *> &notes,
                           SingingClip *clip) override;

    void handleLanguageModuleStatusChanged(AppStatus::ModuleStatus status);
    void handleGetPronTaskFinished(GetPronunciationTask *task);
    void handleGetPhoneTaskFinished(GetPhonemeNameTask *task);
    void handleInferDurTaskFinished(InferDurationTask *task);
    void handleInferPitchTaskFinished(InferPitchTask *task);

    void createAndRunGetPronTask(SingingClip *clip);
    void createAndRunGetPhoneTask(SingingClip *clip);

    void createAndRunInferDurTask(SingingClip *clip);
    void createAndRunInferPitchTask(InferPiece &piece);

    void cancelClipRelatedTasks(const Clip *clip);
    void cancelPieceRelatedTasks(const InferPiece *piece);

    void runNextGetPronTask();
    void runNextGetPhoneTask();
    void runNextInferDurTask();
    void runNextInferPitchTask();

    AppStatus::EditObjectType m_lastEditObjectType = AppStatus::EditObjectType::None;

    QMap<int /*clipId*/, QList<int /*pieceId*/>> m_clipPieceDict;
    QMap<int /*pieceId*/, InferDurationTask::InferDurInput> m_lastInferDurInputs;
    QMap<int /*pieceId*/, InferPitchTask::InferPitchInput> m_lastInferPitchInputs;

    TaskQueue<GetPronunciationTask> m_getPronTasks;
    TaskQueue<GetPhonemeNameTask> m_getPhoneTasks;
    TaskQueue<InferDurationTask> m_inferDurTasks;
    TaskQueue<InferPitchTask> m_inferPitchTasks;

    const QString m_singerConfigPath =
        R"(F:\Sound libraries\DiffSinger\OpenUtau\Singers\Junninghua_v1.4.0_DiffSinger_OpenUtau\dsconfig.yaml)";

private:
    InferController *q_ptr = nullptr;
};



#endif // INFERCONTROLLERPRIVATE_H