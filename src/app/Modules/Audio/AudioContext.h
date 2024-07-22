//
// Created by Crs_1 on 2024/2/4.
//

#ifndef AUDIOCONTEXT_H
#define AUDIOCONTEXT_H

#include <QObject>
#include <QMap>
#include <QTimer>

#include <TalcsCore/PositionableMixerAudioSource.h>
#include <TalcsCore/SmoothedFloat.h>
#include <TalcsCore/AudioSourceClipSeries.h>
#include <TalcsDspx/DspxProjectContext.h>

#include "Controller/PlaybackController.h"
#include "Global/PlaybackGlobal.h"

namespace talcs {
    class DspxAudioClipContext;
}

class AudioContext : public talcs::DspxProjectContext {
    Q_OBJECT
public:
    explicit AudioContext(QObject *parent = nullptr);
    ~AudioContext() override;

    static AudioContext *instance();

    Track *getTrackFromContext(talcs::DspxTrackContext *trackContext) const;
    AudioClip *getAudioClipFromContext(talcs::DspxAudioClipContext *audioClipContext) const;

    talcs::DspxTrackContext *getContextFromTrack(Track *trackModel) const;
    talcs::DspxAudioClipContext *getContextFromAudioClip(AudioClip *audioClipModel) const;

signals:
    void levelMeterUpdated(const AppModel::LevelMetersUpdatedArgs &args);

private:
    QHash<Track *, talcs::DspxTrackContext *> m_trackModelDict;
    QHash<AudioClip *, talcs::DspxAudioClipContext *> m_audioClipModelDict;

    QTimer *m_levelMeterTimer;
    QHash<const Track *, QPair<std::shared_ptr<talcs::SmoothedFloat>, std::shared_ptr<talcs::SmoothedFloat>>> m_trackLevelMeterValue;


    bool m_transportPositionFlag = true;

    void handlePlaybackStatusChanged(PlaybackStatus status);
    void handlePlaybackPositionChanged(double positionTick);

    void handleModelChanged();
    void handleTrackInserted(int index, Track *track);
    void handleTrackRemoved(int index, Track *track);

    void handleTrackControlChanged(Track *track);
    void handleClipInserted(Track *track, int id, AudioClip *audioClip);
    void handleClipRemoved(Track *track, int id, AudioClip *audioClip);

    void handleClipPropertyChanged(AudioClip *audioClip) const;

    void handleTimeChanged();

};

#endif // AUDIOCONTEXT_H
