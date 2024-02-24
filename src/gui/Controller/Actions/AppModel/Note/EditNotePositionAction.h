//
// Created by fluty on 2024/2/8.
//

#ifndef EDITNOTEPOSITIONACTION_H
#define EDITNOTEPOSITIONACTION_H

#include "Controller/History/IAction.h"

class SingingClip;
class Note;

class EditNotePositionAction final : public IAction {
public:
    static EditNotePositionAction *build(Note *note, int deltaTick, int deltaKey,
                                         SingingClip *clip);
    void execute() override;
    void undo() override;

private:
    Note *m_note = nullptr;
    int m_deltaTick = 0;
    int m_deltaKey = 0;
    SingingClip *m_clip = nullptr;
};



#endif // EDITNOTEPOSITIONACTION_H
