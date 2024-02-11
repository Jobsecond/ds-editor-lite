//
// Created by fluty on 2024/1/27.
//

#ifndef DSPXMODEL_H
#define DSPXMODEL_H

#include "Track.h"
#include "Utils/Singleton.h"

class AppModel final : public QObject, public Singleton<AppModel> {
    Q_OBJECT

public:
    explicit AppModel() = default;

    enum TrackChangeType { Insert, PropertyUpdate, Remove };

    class TimeSignature {
    public:
        TimeSignature() = default;
        TimeSignature(int num, int deno) : numerator(num), denominator(deno) {
        }
        int pos = 0;
        int numerator = 4;
        int denominator = 4;
    };

    TimeSignature timeSignature() const;
    void setTimeSignature(const TimeSignature &signature);
    double tempo() const;
    void setTempo(double tempo);
    const QList<Track *> &tracks() const;
    void insertTrack(Track *track, int index);
    void insertTrackQuietly(Track *track, int index);
    void appendTrack(Track *track);
    void removeTrackAt(int index);
    void removeTrack(Track *track);
    void clearTracks();
    int quantize() const;
    void setQuantize(int quantize);

    void newProject();
    bool importMidiFile(const QString &filename);
    bool exportMidiFile(const QString &filename);
    bool loadProject(const QString &filename);
    bool saveProject(const QString &filename);
    bool importAProject(const QString &filename);
    void loadFromAppModel(const AppModel &model);

    int selectedTrackIndex() const;
    void setSelectedTrack(int trackIndex);

    Clip *findClipById(int clipId, int &trackIndex);

    class LevelMetersUpdatedArgs {
    public:
        class State {
        public:
            double valueL = 0;
            double valueR = 0;
        };
        QList<State> trackMeterStates;
    };

public slots:
    void onSelectedClipChanged(int clipId);

signals:
    void modelChanged();
    void tempoChanged(double tempo);
    void timeSignatureChanged(int numerator, int denominator);
    void tracksChanged(TrackChangeType type, int index, Track *track);
    void selectedClipChanged(Track *track, Clip *clip);
    void quantizeChanged(int quantize);
    void selectedTrackChanged(int trackIndex);

private:
    void reset();

    TimeSignature m_timeSignature;
    double m_tempo = 120;
    QList<Track *> m_tracks;

    int m_selectedTrackIndex = -1;
    int m_selectedClipId = -1;

    int m_quantize = 16;
};



#endif // DSPXMODEL_H
