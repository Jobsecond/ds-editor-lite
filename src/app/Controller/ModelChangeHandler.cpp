//
// Created by fluty on 24-9-29.
//

#include "ModelChangeHandler.h"

ModelChangeHandler::ModelChangeHandler(QObject *parent) : QObject(parent) {
    connect(appModel, &AppModel::modelChanged, this, &ModelChangeHandler::onModelChanged);
}

void ModelChangeHandler::handleTrackInserted(Track *track) {
    m_tracks.append(track);
    for (const auto clip : track->clips())
        handleClipInserted(clip);
    connect(track, &Track::clipChanged, this, &ModelChangeHandler::onClipChanged);
}

void ModelChangeHandler::handleTrackRemoved(Track *track) {
    m_tracks.removeOne(track);
    for (const auto clip : track->clips())
        handleClipRemoved(clip);
    disconnect(track, &Track::clipChanged, this, &ModelChangeHandler::onClipChanged);
}

void ModelChangeHandler::handleClipInserted(Clip *clip) {
    if (clip->clipType() == IClip::Singing)
        handleSingingClipInserted(reinterpret_cast<SingingClip *>(clip));
}

void ModelChangeHandler::handleClipRemoved(Clip *clip) {
    if (clip->clipType() == IClip::Singing)
        handleSingingClipRemoved(reinterpret_cast<SingingClip *>(clip));
    disconnect(clip, nullptr, this, nullptr);
}

void ModelChangeHandler::handleSingingClipInserted(SingingClip *clip) {
    connect(clip, &SingingClip::noteChanged, this,
            [=](SingingClip::NoteChangeType type, const QList<Note *> &notes) {
                handleNoteChanged(type, notes, clip);
            });
}

void ModelChangeHandler::handleSingingClipRemoved(SingingClip *clip) {
}

void ModelChangeHandler::handleNoteChanged(SingingClip::NoteChangeType type,
                                           const QList<Note *> &notes, SingingClip *clip) {
}

void ModelChangeHandler::handleParamChanged(ParamInfo::Name name, Param::Type type,
                                            SingingClip *clip) {
}

void ModelChangeHandler::handlePiecesChanged(const QList<InferPiece *> &pieces, SingingClip *clip) {
}

void ModelChangeHandler::onModelChanged() {
    for (const auto track : m_tracks)
        onTrackChanged(AppModel::Remove, -1, track);

    for (const auto track : appModel->tracks())
        onTrackChanged(AppModel::Insert, -1, track);
}

void ModelChangeHandler::onTrackChanged(AppModel::TrackChangeType type, qsizetype index,
                                        Track *track) {
    if (type == AppModel::Insert)
        handleTrackInserted(track);
    else if (type == AppModel::Remove)
        handleTrackRemoved(track);
}

void ModelChangeHandler::onClipChanged(Track::ClipChangeType type, Clip *clip) {
    if (type == Track::Inserted)
        handleClipInserted(clip);
    else if (type == Track::Removed)
        handleClipRemoved(clip);
}