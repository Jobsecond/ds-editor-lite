//
// Created by fluty on 2024/2/8.
//

#ifndef REMOVENOTEACTION_H
#define REMOVENOTEACTION_H

#include "Modules/History/IAction.h"

class SingingClip;
class Note;

class RemoveNoteAction final : public IAction {
public:
    static RemoveNoteAction *build(Note *note, SingingClip *clip);
    void execute() override;
    void undo() override;

private:
    Note *m_note = nullptr;
    SingingClip *m_clip = nullptr;
};



#endif // REMOVENOTEACTION_H
