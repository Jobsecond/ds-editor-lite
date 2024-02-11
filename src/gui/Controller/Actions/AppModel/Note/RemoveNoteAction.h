//
// Created by fluty on 2024/2/8.
//

#ifndef REMOVENOTEACTION_H
#define REMOVENOTEACTION_H

#include "Controller/History/IAction.h"
#include "Model/Clip.h"
#include "Model/Note.h"

class RemoveNoteAction final : public IAction {
public:
    static RemoveNoteAction *build(Note *note, DsSingingClip *clip);
    void execute() override;
    void undo() override;

private:
    Note *m_note = nullptr;
    DsSingingClip *m_clip = nullptr;
};



#endif // REMOVENOTEACTION_H
