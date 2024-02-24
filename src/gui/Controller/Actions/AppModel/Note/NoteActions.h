//
// Created by fluty on 2024/2/8.
//

#ifndef NOTEACTIONS_H
#define NOTEACTIONS_H

#include "Controller/History/ActionSequence.h"
#include "Model/Note.h"

class SingingClip;

class NoteActions : public ActionSequence {
public:
    void insertNotes(const QList<Note *> &notes, SingingClip *clip);
    void removeNotes(const QList<Note *> &notes, SingingClip *clip);

    // Resize from left
    void editNotesStartAndLength(const QList<Note *> &notes, int delta, SingingClip *clip);

    // Resize from right
    void editNotesLength(const QList<Note *> &notes, int delta, SingingClip *clip);

    // Move notes
    void editNotePosition(const QList<Note *> &notes, int deltaTick, int deltaKey,
                          SingingClip *clip);

    // Edit lyrics, pronunciations and phonemes
    void editNotesWordProperties(const QList<Note *> &notes,
                                 const QList<Note::NoteWordProperties *> &args,
                                 SingingClip *clip);
    void editNotesPhoneme(const QList<Note *> &notes, const QList<Phoneme> &phonemes, SingingClip *clip);
};



#endif // NOTEACTIONS_H
