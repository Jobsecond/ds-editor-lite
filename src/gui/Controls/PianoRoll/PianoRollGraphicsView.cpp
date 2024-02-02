//
// Created by fluty on 2024/1/23.
//

#include <QDebug>
#include <QScroller>

#include "NoteGraphicsItem.h"
#include "PianoRollBackgroundGraphicsItem.h"
#include "PianoRollGlobal.h"
#include "PianoRollGraphicsScene.h"
#include "PianoRollGraphicsView.h"

PianoRollGraphicsView::PianoRollGraphicsView() {
    setScaleXMax(5);
    // QScroller::grabGesture(this, QScroller::TouchGesture);
    setDragMode(RubberBandDrag);

    m_pianoRollScene = new PianoRollGraphicsScene;
    // setScene(m_pianoRollScene);
    connect(this, &PianoRollGraphicsView::scaleChanged, m_pianoRollScene, &PianoRollGraphicsScene::setScale);

    auto gridItem = new PianoRollBackgroundGraphicsItem;
    gridItem->setPixelsPerQuarterNote(PianoRollGlobal::pixelsPerQuarterNote);
    connect(this, &PianoRollGraphicsView::visibleRectChanged, gridItem, &TimeGridGraphicsItem::setVisibleRect);
    connect(this, &PianoRollGraphicsView::scaleChanged, gridItem, &PianoRollBackgroundGraphicsItem::setScale);
    auto appModel = AppModel::instance();
    connect(appModel, &AppModel::modelChanged, gridItem, [=] {
        gridItem->onTimeSignatureChanged(appModel->numerator, appModel->denominator);
    });
    connect(appModel, &AppModel::timeSignatureChanged, gridItem, &TimeGridGraphicsItem::onTimeSignatureChanged);
    m_pianoRollScene->addItem(gridItem);

    // auto pitchItem = new PitchEditorGraphicsItem;
    // QObject::connect(pianoRollView, &TracksGraphicsView::visibleRectChanged, pitchItem,
    //                  &PitchEditorGraphicsItem::setVisibleRect);
    // QObject::connect(pianoRollView, &TracksGraphicsView::scaleChanged, pitchItem,
    // &PitchEditorGraphicsItem::setScale); pianoRollScene->addItem(pitchItem);
}
void PianoRollGraphicsView::updateView() {
    onSelectedClipChanged(-1, -1);
}
void PianoRollGraphicsView::onSelectedClipChanged(int trackIndex, int clipIndex) {
    reset();

    auto model = AppModel::instance();
    if (trackIndex == -1 || clipIndex == -1) {
        m_oneSingingClipSelected = false;
        setScene(nullptr);
        update();
        return;
    }

    auto clip = model->tracks().at(trackIndex)->clips().at(clipIndex);
    if (clip->type() != DsClip::Singing) {
        m_oneSingingClipSelected = false;
        setScene(nullptr);
        update();
        return;
    }

    m_oneSingingClipSelected = true;
    setScene(m_pianoRollScene);
    update();
    auto singingClip = dynamic_cast<DsSingingClip *>(clip);
    for (int i = 0; i < singingClip->notes.count(); i++) {
        auto note = singingClip->notes.at(i);
        insertNote(note, i);
    }
    auto firstNoteItem = m_noteItems.first();
    centerOn(firstNoteItem);
}
void PianoRollGraphicsView::paintEvent(QPaintEvent *event) {
    CommonGraphicsView::paintEvent(event);

    if (m_oneSingingClipSelected)
        return;

    QPainter painter(viewport());
    painter.setPen(QColor(160, 160, 160));
    painter.drawText(viewport()->rect(), "Select a singing clip to edit", QTextOption(Qt::AlignCenter));
}
void PianoRollGraphicsView::reset() {
    for (auto note : m_noteItems) {
        m_pianoRollScene->removeItem(note);
        delete note;
    }
    m_noteItems.clear();
}
void PianoRollGraphicsView::insertNote(const DsNote &dsNote, int index) {
    auto noteItem = new NoteGraphicsItem(0);
    noteItem->setStart(dsNote.start());
    noteItem->setLength(dsNote.length());
    noteItem->setKeyIndex(dsNote.keyIndex());
    noteItem->setLyric(dsNote.lyric());
    noteItem->setPronunciation(dsNote.pronunciation());
    noteItem->setVisibleRect(this->visibleRect());
    noteItem->setScaleX(this->scaleX());
    noteItem->setScaleY(this->scaleY());
    m_pianoRollScene->addItem(noteItem);
    connect(this, &PianoRollGraphicsView::scaleChanged, noteItem, &NoteGraphicsItem::setScale);
    connect(this, &PianoRollGraphicsView::visibleRectChanged, noteItem, &NoteGraphicsItem::setVisibleRect);
    m_noteItems.append(noteItem);
}