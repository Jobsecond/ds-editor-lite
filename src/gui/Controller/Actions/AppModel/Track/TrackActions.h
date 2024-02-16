//
// Created by fluty on 2024/2/8.
//

#ifndef TRACKACTIONS_H
#define TRACKACTIONS_H

#include "Controller/History/ActionSequence.h"
#include "Model/AppModel.h"

class TrackActions : public ActionSequence {
public:
    void appendTracks(const QList<Track *> &tracks, AppModel *model);
    void insertTrack(Track *track, int index, AppModel *model);
    void removeTracks(const QList<Track *> &tracks, AppModel *model);
    void editTrackProperties(const Track::TrackProperties &oldArgs,
                             const Track::TrackProperties &newArgs, Track *track);
};



#endif // TRACKACTIONS_H