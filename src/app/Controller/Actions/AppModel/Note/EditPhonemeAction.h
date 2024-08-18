//
// Created by fluty on 24-2-14.
//

#ifndef EDITPHONEMEACTION_H
#define EDITPHONEMEACTION_H

#include "Model/AppModel/Note.h"
#include "Modules/History/IAction.h"

class SingingClip;
class Note;

class EditPhonemeAction final : public IAction {
public:
    static EditPhonemeAction *build(Note *note, const QList<Phoneme> &phonemes);
    void execute() override;
    void undo() override;

private:
    Note *m_note = nullptr;
    QList<Phoneme> m_oldPhonemes;
    QList<Phoneme> m_newPhonemes;
};



#endif // EDITPHONEMEACTION_H
