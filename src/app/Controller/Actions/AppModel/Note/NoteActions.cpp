//
// Created by fluty on 2024/2/8.
//

#include "NoteActions.h"

#include "EditNotePositionAction.h"
#include "EditNoteStartAndLengthAction.h"
#include "EditNotesLengthAction.h"
#include "EditNoteWordPropertiesAction.h"
#include "EditPhonemeAction.h"
#include "InsertNoteAction.h"
#include "RemoveNoteAction.h"

void NoteActions::insertNotes(const QList<Note *> &notes, SingingClip *clip) {
    setName(tr("Insert note(s)"));
    addAction(new InsertNoteAction(notes, clip));
}

void NoteActions::removeNotes(const QList<Note *> &notes, SingingClip *clip) {
    setName(tr("Remove note(s)"));
    addAction(new RemoveNoteAction(notes, clip));
}

void NoteActions::editNotesStartAndLength(const QList<Note *> &notes, int delta,
                                          SingingClip *clip) {
    setName(tr("Edit note start and length"));
    addAction(new EditNoteStartAndLengthAction(notes, delta, clip));
}

void NoteActions::editNotesLength(const QList<Note *> &notes, int delta, SingingClip *clip) {
    setName(tr("Edit note length"));
    addAction(new EditNotesLengthAction(notes, delta, clip));
}

void NoteActions::editNotePosition(const QList<Note *> &notes, int deltaTick, int deltaKey,
                                   SingingClip *clip) {
    setName(tr("Edit note position"));
    addAction(new EditNotePositionAction(notes, deltaTick, deltaKey, clip));
}

void NoteActions::editNotesWordProperties(const QList<Note *> &notes,
                                          const QList<Note::WordProperties> &args,
                                          SingingClip *clip) {
    setName(tr("Edit note word properties"));
    addAction(new EditNoteWordPropertiesAction(notes, args, clip));
}

void NoteActions::editNotesPhoneme(Note *note, const QList<Phoneme> &phonemes, SingingClip *clip) {
    setName(tr("Edit note phoneme"));
    addAction(new EditPhonemeAction(note, phonemes, clip));
}