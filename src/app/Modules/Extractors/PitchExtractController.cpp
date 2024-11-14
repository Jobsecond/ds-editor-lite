//
// Created by fluty on 24-11-13.
//

#include "PitchExtractController.h"

#include "ExtractPitchTask.h"
#include "Controller/Actions/AppModel/Param/ParamsActions.h"
#include "Model/AppModel/AudioClip.h"
#include "Model/AppModel/DrawCurve.h"
#include "Modules/History/HistoryManager.h"
#include "Modules/Task/TaskManager.h"
#include "UI/Controls/AccentButton.h"
#include "UI/Controls/Toast.h"
#include "UI/Dialogs/Base/TaskDialog.h"
#include "Utils/Linq.h"

void PitchExtractController::runExtractPitch(AudioClip *audioClip, SingingClip *singingClip) {
    auto path = audioClip->path();
    auto task = new ExtractPitchTask({singingClip->id(), audioClip->id(), path, appModel->tempo()});
    auto dlg = new TaskDialog(task, true, true);
    dlg->show();
    connect(task, &Task::finished, this, [=] { onExtractPitchTaskFinished(task); });
    taskManager->addAndStartTask(task);
}

void PitchExtractController::onExtractPitchTaskFinished(ExtractPitchTask *task) {
    taskManager->removeTask(task);
    if (!task->success) {
        Dialog dialog;
        dialog.setTitle("Task Failed");
        dialog.setMessage(
            tr("Failed to extract pitch from audio:\n %1").arg(task->input().audioPath));
        dialog.setModal(true);

        auto btnClose = new AccentButton(tr("Close"));
        connect(btnClose, &Button::clicked, &dialog, &Dialog::accept);
        dialog.setPositiveButton(btnClose);
        dialog.exec();
        delete task;
        return;
    }

    auto audioClip = dynamic_cast<AudioClip *>(appModel->findClipById(task->input().audioClipId));
    auto singingClip =
        dynamic_cast<SingingClip *>(appModel->findClipById(task->input().singingClipId));
    if (!audioClip || !singingClip) {
        delete task;
        return;
    }

    auto pitchParam = new DrawCurve;
    pitchParam->setStart(audioClip->start());
    pitchParam->setValues(Linq::selectMany(task->result, L_PRED(v, static_cast<int>(v * 100))));
    pitchParam->erase(audioClip->start(), audioClip->start() + audioClip->clipStart());
    pitchParam->eraseTailFrom(audioClip->endTick());

    if (pitchParam->start() < singingClip->start() + singingClip->clipStart())
        pitchParam->erase(pitchParam->start(), singingClip->start() + singingClip->clipStart());
    if (pitchParam->endTick() > singingClip->endTick())
        pitchParam->eraseTailFrom(singingClip->endTick());
    pitchParam->setStart(pitchParam->start() - singingClip->start());

    auto a = new ParamsActions;
    a->replaceParam(ParamInfo::Pitch, Param::Edited, {pitchParam}, singingClip);
    a->execute();
    historyManager->record(a);

    delete task;
}