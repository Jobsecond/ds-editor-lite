//
// Created by fluty on 2024/2/8.
//

#ifndef INSERTCLIPACTION_H
#define INSERTCLIPACTION_H

#include "Modules/History/IAction.h"

class Track;
class Clip;

class InsertClipAction final : public IAction {
public:
    static InsertClipAction *build(Clip *clip, Track *track);
    void execute() override;
    void undo() override;

private:
    Clip *m_clip = nullptr;
    Track *m_track = nullptr;
};



#endif // INSERTCLIPACTION_H
