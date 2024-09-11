//
// Created by fluty on 24-2-12.
//

#ifndef PHONEMEPARAMEDITOR_H
#define PHONEMEPARAMEDITOR_H

#include <QWidget>

#include "UI/Utils/ITimelinePainter.h"
#include "Utils/OverlappableSerialList.h"
#include "Utils/Overlappable.h"
#include "Model/AppModel/Clip.h"

class Note;
class Phoneme;

class PhonemeView final : public QWidget, public ITimelinePainter {
    Q_OBJECT

public:
    explicit PhonemeView(QWidget *parent = nullptr);
    void setDataContext(SingingClip *clip);

    class PhonemeViewModel {
    public:
        enum PhonemeType { Ahead, Normal, Final, Sil };

        PhonemeType type;
        int noteId = -1;
        int noteStart = 0;
        int noteLength = 0;
        int start = 0;
        QString name;
        bool nameEdited = false;
        bool offsetEdited = false;
        bool offsetReady = false;
        bool hoverOnControlBar = false;
        int startOffset = 0;

        PhonemeViewModel *prior = nullptr;
        PhonemeViewModel *next = nullptr;
    };

signals:
    void adjustCompleted(PhonemeView::PhonemeViewModel *phonemeViewModel);

public slots:
    void setTimeRange(double startTick, double endTick);
    void setTimeSignature(int numerator, int denominator) override;
    void setPosition(double tick);
    void setQuantize(int quantize) override;

private slots:
    void onTempoChanged(double tempo);
    void onClipPropertyChanged();
    void onNoteChanged(SingingClip::NoteChangeType type, const QList<Note *> &notes);
    // void onNoteSelectionChanged();

private:
    enum MouseMoveBehavior { Move, None };

    void paintEvent(QPaintEvent *event) override;
    void drawBar(QPainter *painter, int tick, int bar) override;
    void drawBeat(QPainter *painter, int tick, int bar, int beat) override;
    void drawEighth(QPainter *painter, int tick) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

    void moveToSingingClipState(SingingClip *clip);
    void moveToNullClipState();
    void updateHoverEffects();
    void updateNoteTime(Note *note);
    void reset();
    double tickToX(double tick);
    double xToTick(double x);
    [[nodiscard]] double ticksPerPixel() const;
    [[nodiscard]] bool canEdit() const;
    double m_startTick = 0;
    double m_endTick = 0;
    double m_resizeToleranceInTick = 0;
    double m_position = 0;
    QList<Note *> m_notes;
    QList<PhonemeViewModel *> m_phonemes;
    MouseMoveBehavior m_mouseMoveBehavior = None;
    PhonemeViewModel *m_curPhoneme = nullptr;
    int m_mouseDownX = 0;
    int m_currentLengthInMs = 0;
    bool m_freezeHoverEffects = false;
    bool m_showDebugInfo = false;
    int m_canEditTicksPerPixelThreshold = 6;
    bool m_mouseMoved = false;

    SingingClip *m_clip = nullptr;

    PhonemeViewModel *phonemeAtTick(double tick);
    QList<PhonemeViewModel *> findPhonemesByNoteId(int noteId);
    void buildPhonemeList();
    void resetPhonemeList();
    void clearHoverEffects(PhonemeViewModel *except = nullptr);
    void handleAdjustCompleted(PhonemeViewModel *phVm);
};



#endif // PHONEMEPARAMEDITOR_H
