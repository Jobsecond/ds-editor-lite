//
// Created by fluty on 2024/2/8.
//

#include "AppendTrackAction.h"

#include "Model/AppModel/AppModel.h"

AppendTrackAction *AppendTrackAction::build(Track *track, AppModel *model) {
    auto a = new AppendTrackAction;
    a->m_track = track;
    a->m_model = model;
    return a;
}
void AppendTrackAction::execute() {
    m_model->appendTrack(m_track);
}
void AppendTrackAction::undo() {
    m_model->removeTrack(m_track);
}