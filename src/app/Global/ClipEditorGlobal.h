//
// Created by fluty on 2024/1/26.
//

#ifndef PIANOROLLGLOBAL_H
#define PIANOROLLGLOBAL_H

#include <QString>

namespace ClipEditorGlobal {
    constexpr int pixelsPerQuarterNote = 96;
    constexpr double noteHeight = 24;
    constexpr int timelineViewHeight = 24;

    enum PianoRollEditMode { Select, DrawNote, DrawPitch, EditPitchAnchor };
    enum NoteEditMode { NotePosition, Length, StartAndLength };
    constexpr int phonemeEditorHeight = 40;
}

#endif //PIANOROLLGLOBAL_H
