//
// Created by fluty on 2024/2/8.
//

#include "InsertNoteAction.h"

InsertNoteAction *InsertNoteAction::build(DsNote *note, DsSingingClip *clip) {
    auto a = new InsertNoteAction;
    a->m_note = note;
    a->m_clip = clip;
    return a;
}
void InsertNoteAction::execute() {
    m_clip->insertNote(m_note);
}
void InsertNoteAction::undo() {
    m_clip->removeNote(m_note);
}