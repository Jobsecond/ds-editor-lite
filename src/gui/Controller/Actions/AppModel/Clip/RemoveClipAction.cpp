//
// Created by fluty on 2024/2/8.
//

#include "RemoveClipAction.h"
RemoveClipAction *RemoveClipAction::build(DsClip *clip, DsTrack *track) {
    auto a = new RemoveClipAction;
    a->m_clip = clip;
    a->m_track = track;
    return a;
}
void RemoveClipAction::execute() {
    m_track->removeClip(m_clip);
}
void RemoveClipAction::undo() {
    m_track->insertClip(m_clip);
}