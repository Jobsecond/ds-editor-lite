//
// Created by fluty on 2024/1/29.
//

#ifndef TRACKSVIEW_H
#define TRACKSVIEW_H

#include "Model/AppModel.h"
#include "Model/Track.h"
#include "Model/Clip.h"
#include "UI/Views/Common/PanelView.h"

class TrackListWidget;
class TracksGraphicsView;
class TracksGraphicsScene;
class TimelineView;
class TracksBackgroundGraphicsItem;
class TrackViewModel;
class AbstractClipGraphicsItem;

class TrackEditorView final : public PanelView {
    Q_OBJECT

public:
    explicit TrackEditorView(QWidget *parent = nullptr);

    AbstractClipGraphicsItem *findClipItemById(int id);

public slots:
    void onModelChanged();
    void onTempoChanged(double tempo);
    void onTrackChanged(AppModel::TrackChangeType type, int index);
    void onClipChanged(Track::ClipChangeType type, Clip *clip);
    void onPositionChanged(double tick);
    void onLastPositionChanged(double tick);
    void onLevelMetersUpdated(const AppModel::LevelMetersUpdatedArgs &args) const;

signals:
    void selectedClipChanged(int clipId);
    void insertNewTrackTriggered(int index);
    void removeTrackTriggered(int index);
    void muteClicked(int index);
    void soloClicked(int index);
    void tempoChanged(double tempo);
    void trackCountChanged(qsizetype count);
    void addAudioClipTriggered(const QString &path, int trackIndex, int tick);
    void clipPropertyChanged(const Clip::ClipCommonProperties &args);
    void setPositionTriggered(double tick);
    void removeClipTriggered(int clipId);
    void newSingingClipTriggered(int trackIndex, int tick);

private slots:
    void onSceneSelectionChanged();
    void onViewScaleChanged(qreal sx, qreal sy);
    void onClipGraphicsItemRemoveTriggered(int id);

private:
    bool eventFilter(QObject *watched, QEvent *event) override;

    TrackListWidget *m_trackListWidget;
    TracksGraphicsView *m_graphicsView;
    TracksGraphicsScene *m_tracksScene;
    TimelineView *m_timeline;
    TracksBackgroundGraphicsItem *m_gridItem;

    class TrackListViewModel {
    public:
        QList<TrackViewModel *> tracks;

        TrackViewModel *findTrackById(int id);
    };

    TrackListViewModel m_trackListViewModel;
    double m_tempo = 120;
    int m_samplerate = 48000;

    void insertTrackToView(Track *dsTrack, int trackIndex);
    void insertClipToTrack(Clip *clip, TrackViewModel *track, int trackIndex);
    void removeClipFromView(int clipId);
    void updateTracksOnView() const;
    void updateClipOnView(Clip *clip);
    void removeTrackFromView(int index);
    void updateOverlappedState();
    void reset();
};



#endif // TRACKSVIEW_H
