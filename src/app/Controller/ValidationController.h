//
// Created by fluty on 2024/7/17.
//

#ifndef VALIDATIONCONTROLLER_H
#define VALIDATIONCONTROLLER_H

#include "Model/AppModel.h"
#include "Model/Track.h"
#include "Utils/Singleton.h"

class Note;
class ValidationController : public QObject, public Singleton<ValidationController> {
    Q_OBJECT
public:
    explicit ValidationController();
    void runValidation();

private slots:
    void onModelChanged();
    void onTempoChanged(double tempo);
    void onTrackChanged(AppModel::TrackChangeType type, qsizetype index, Track *track);
    void onClipChanged(Track::ClipChangeType type, int id, Clip *clip);
    void onClipPropertyChanged(Clip *clip);

signals:
    void validationFinished(bool passed);

private:
    void validate();
    static bool validateProjectLength();
    static bool validateTempo();
    bool validateClipOverlap();
    bool validateNoteOverlap();

    QList<Track *> m_tracks;
    QList<Clip *> m_clips;
};



#endif // VALIDATIONCONTROLLER_H
