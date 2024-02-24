//
// Created by fluty on 2024/1/23.
//

#ifndef PIANOROLLGRAPHICSVIEW_H
#define PIANOROLLGRAPHICSVIEW_H

#include "ClipEditorGlobal.h"
#include "Model/Params.h"
#include "Views/Common/TimeGraphicsView.h"

class Note;
class PianoRollGraphicsScene;
class PitchEditorGraphicsItem;
class SingingClip;
class NoteGraphicsItem;

using namespace ClipEditorGlobal;

class PianoRollGraphicsView final : public TimeGraphicsView {
    Q_OBJECT

public:
    explicit PianoRollGraphicsView(PianoRollGraphicsScene *scene);
    void setIsSingingClip(bool isSingingClip);
    void setEditMode(PianoRollEditMode mode);
    void insertNote(Note *note);
    void removeNote(int noteId);
    void updateNoteTimeAndKey(Note *note);
    void updateNoteWord(Note *note);
    void updateNoteSelection(const QList<Note *> &selectedNotes);
    void reset();
    QList<int> selectedNotesId() const;
    void updateOverlappedState(SingingClip *singingClip);
    void clearNoteSelections(NoteGraphicsItem *except = nullptr);
    void updatePitch(Param::ParamType paramType, const Param &param);

    double topKeyIndex() const;
    double bottomKeyIndex() const;
    void setViewportTopKey(double key);
    void setViewportCenterAt(double tick, double keyIndex);
    void setViewportCenterAtKeyIndex(double keyIndex);

signals:
    void noteShapeEdited(NoteEditMode mode, int deltaTick, int deltaKey);
    void removeNoteTriggered();
    void editNoteLyricTriggered();
    void drawNoteCompleted(int start, int length, int keyIndex);
    void moveNotesCompleted(int deltaTick, int deltaKey);
    void resizeNoteLeftCompleted(int noteId, int deltaTick);
    void resizeNoteRightCompleted(int noteId, int deltaTick);
    void selectedNoteChanged(QList<int> notes);
    void pitchEdited(const OverlapableSerialList<Curve> &curves);

public slots:
    void onSceneSelectionChanged();
    void onPitchEditorEditCompleted();

private:
    void paintEvent(QPaintEvent *event) override;
    void prepareForMovingOrResizingNotes(QMouseEvent *event, QPointF scenePos, int keyIndex,
                                         NoteGraphicsItem *noteItem);
    void PrepareForDrawingNote(int tick, int keyIndex);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    enum MouseMoveBehavior { ResizeLeft, Move, ResizeRight, UpdateDrawingNote, None };
    NoteGraphicsItem *m_currentEditingNote = nullptr;

    PitchEditorGraphicsItem *m_pitchItem;

    bool m_selecting = false;
    QList<Note *> m_cachedSelectedNotes;
    void updateSelectionState();
    bool m_canNotifySelectedNoteChanged = true;

    // resize and move
    bool m_tempQuantizeOff = false;
    QPointF m_mouseDownPos;
    int m_mouseDownStart;
    int m_mouseDownLength;
    int m_mouseDownKeyIndex;
    int m_deltaTick = 0;
    int m_deltaKey = 0;
    bool m_movedBeforeMouseUp = false;
    int m_moveMaxDeltaKey = 127;
    int m_moveMinDeltaKey = 0;
    void moveSelectedNotes(int startOffset, int keyOffset);
    void resetSelectedNotesOffset();
    void updateMoveDeltaKeyRange();
    void resetMoveDeltaKeyRange();
    void resizeLeftSelectedNote(int offset);
    void resizeRightSelectedNote(int offset);

    bool m_isSingingClipSelected = false;
    PianoRollEditMode m_mode = Select;
    MouseMoveBehavior m_mouseMoveBehavior = None;
    QList<NoteGraphicsItem *> m_noteItems;
    NoteGraphicsItem *m_currentDrawingNote; // a fake note for drawing

    NoteGraphicsItem *findNoteById(int id);
    double keyIndexToSceneY(double index) const;
    double sceneYToKeyIndexDouble(double y) const;
    int sceneYToKeyIndexInt(double y) const;
    QList<NoteGraphicsItem *> selectedNoteItems() const;
    void setPitchEditMode(bool on);
    NoteGraphicsItem *noteItemAt(const QPoint &pos);
    // OverlapableSerialList<Curve> buildCurves();
};

#endif // PIANOROLLGRAPHICSVIEW_H
