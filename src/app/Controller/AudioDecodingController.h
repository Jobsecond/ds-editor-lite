//
// Created by fluty on 2024/7/11.
//

#ifndef AUDIODECODINGCONTROLLER_H
#define AUDIODECODINGCONTROLLER_H

#include "Utils/Singleton.h"
#include "Model/AppModel.h"
#include "Model/Track.h"

#include <QObject>

class AudioClip;
class DecodeAudioTask;
class AudioDecodingController final : public QObject, public Singleton<AudioDecodingController> {
    Q_OBJECT

public slots:
    void onModelChanged();
    void onTrackChanged(AppModel::TrackChangeType type, qsizetype index, Track *track);
    void onClipChanged(Track::ClipChangeType type, int id, Clip *clip);

private:
    QList<DecodeAudioTask *> m_tasks;

    void createAndStartTask(AudioClip *clip);
    void handleTaskFinished(DecodeAudioTask *task, bool terminate);
    void terminateTaskByClipId(int clipId);
    void terminateTasksByTrackId(int trackId);
};

#endif // AUDIODECODINGCONTROLLER_H
