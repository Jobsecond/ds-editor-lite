//
// Created by FlutyDeer on 2023/12/1.
//

#ifndef TRACKSCONTROLLER_H
#define TRACKSCONTROLLER_H

#include <QObject>

#include "Controls/PianoRoll/PianoRollGraphicsView.h"
#include "Utils/Singleton.h"
#include "Views/TracksView.h"

class AppController final : public QObject, public Singleton<AppController>{
    Q_OBJECT

public:
    explicit AppController();
    ~AppController() override;

    TracksView *tracksView() const;
    PianoRollGraphicsView *pianoRollView() const;

public slots:
    void onNewTrack();
    void onInsertNewTrack(int index);
    void onRemoveTrack(int index);
    void openProject(const QString &filePath);
    void addAudioClipToNewTrack(const QString &filePath);
    void onSelectedClipChanged(int trackIndex, int clipIndex);
    void onTrackPropertyChanged(const QString &name, const DsTrackControl &control, int index);

private:

    // Views
    TracksView *m_tracksView;
    PianoRollGraphicsView *m_pianoRollView;
};

// using ControllerSingleton = Singleton<Controller>;

#endif // TRACKSCONTROLLER_H
