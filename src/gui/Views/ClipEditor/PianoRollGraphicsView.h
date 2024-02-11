//
// Created by fluty on 2024/1/23.
//

#ifndef PIANOROLLGRAPHICSVIEW_H
#define PIANOROLLGRAPHICSVIEW_H

#include "PianoRollGlobal.h"
#include "GraphicsItem/NoteGraphicsItem.h"
#include "Model/Clip.h"
#include "Model/Note.h"
#include "Views/Common/TimeGraphicsView.h"

using namespace PianoRollGlobal;

class PianoRollGraphicsView final : public TimeGraphicsView {
    Q_OBJECT

public:
    explicit PianoRollGraphicsView(PianoRollGraphicsScene *scene);
    void setIsSingingClip(bool isSingingClip);
    void setEditMode(PianoRollEditMode mode);
    void insertNote(Note *dsNote);
    void removeNote(int noteId);
    void updateNote(Note *note);
    void reset();
    QList<int> selectedNotesId() const;
    void updateOverlappedState(SingingClip *singingClip);

    double topKeyIndex()const;
    double bottomKeyIndex() const;
    void setViewportTopKey(double key);
    void setViewportCenterAt(double tick, double keyIndex);
    void setViewportCenterAtKeyIndex(double keyIndex);

signals:
    void noteShapeEdited(NoteEditMode mode, int deltaTick, int deltaKey);
    void removeNoteTriggered();
    void editNoteLyricTriggered();
    void drawNoteCompleted(int start, int length, int keyIndex);

private:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    enum MouseMoveBehavior { ResizeLeft, Move, ResizeRight, UpdateDrawingNote, None };

    bool m_isSingingClipSelected = false;
    PianoRollEditMode m_mode = Select;
    MouseMoveBehavior m_mouseMoveBehavior = None;
    QList<NoteGraphicsItem *> m_noteItems;
    NoteGraphicsItem *m_currentDrawingNote; // a fake note for drawing

    NoteGraphicsItem *findNoteById(int id);
    double keyIndexToSceneY(double index) const;
    double sceneYToKeyIndexDouble(double y) const;
    int sceneYToKeyIndexInt(double y) const;
};

#endif // PIANOROLLGRAPHICSVIEW_H
