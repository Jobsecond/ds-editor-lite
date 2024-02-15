//
// Created by fluty on 2024/2/10.
//

#include <QVBoxLayout>

#include "ClipEditorView.h"
#include "PianoRollGraphicsScene.h"
#include "Controller/ClipEditorViewController.h"
#include "Controller/PlaybackController.h"
#include "Controller/TracksViewController.h"
#include "GraphicsItem/PianoRollBackgroundGraphicsItem.h"
#include "Model/AppModel.h"
#include "Views/Common/TimelineView.h"

ClipEditorView::ClipEditorView(QWidget *parent) : QWidget(parent) {
    m_toolbarView = new ClipEditorToolBarView;
    connect(m_toolbarView, &ClipEditorToolBarView::clipNameChanged, this,
            &ClipEditorView::onClipNameEdited);
    connect(m_toolbarView, &ClipEditorToolBarView::editModeChanged, this,
            &ClipEditorView::onEditModeChanged);

    // auto pitchItem = new PitchEditorGraphicsItem;
    // QObject::connect(pianoRollView, &TracksGraphicsView::visibleRectChanged, pitchItem,
    //                  &PitchEditorGraphicsItem::setVisibleRect);
    // QObject::connect(pianoRollView, &TracksGraphicsView::scaleChanged, pitchItem,
    // &PitchEditorGraphicsItem::setScale); pianoRollScene->addItem(pitchItem);

    m_pianoRollScene = new PianoRollGraphicsScene;
    m_pianoRollView = new PianoRollGraphicsView(m_pianoRollScene);
    m_pianoRollView->setSceneVisibility(false);
    m_pianoRollView->setDragMode(QGraphicsView::RubberBandDrag);
    m_pianoRollView->setPixelsPerQuarterNote(ClipEditorGlobal::pixelsPerQuarterNote);
    connect(m_pianoRollView, &PianoRollGraphicsView::scaleChanged, m_pianoRollScene,
            &PianoRollGraphicsScene::setScale);

    auto gridItem = new PianoRollBackgroundGraphicsItem;
    gridItem->setPixelsPerQuarterNote(ClipEditorGlobal::pixelsPerQuarterNote);
    connect(m_pianoRollView, &PianoRollGraphicsView::visibleRectChanged, gridItem,
            &TimeGridGraphicsItem::setVisibleRect);
    connect(m_pianoRollView, &PianoRollGraphicsView::scaleChanged, gridItem,
            &PianoRollBackgroundGraphicsItem::setScale);
    auto appModel = AppModel::instance();
    connect(appModel, &AppModel::modelChanged, gridItem, [=] {
        gridItem->setTimeSignature(appModel->timeSignature().numerator,
                                   appModel->timeSignature().denominator);
    });
    connect(appModel, &AppModel::timeSignatureChanged, gridItem,
            &TimeGridGraphicsItem::setTimeSignature);
    m_pianoRollScene->addTimeGrid(gridItem);

    m_timelineView = new TimelineView;
    m_timelineView->setTimeRange(m_pianoRollView->startTick(), m_pianoRollView->endTick());
    m_timelineView->setPixelsPerQuarterNote(ClipEditorGlobal::pixelsPerQuarterNote);
    m_timelineView->setFixedHeight(timelineViewHeight);
    m_timelineView->setVisible(false);
    connect(m_timelineView, &TimelineView::wheelHorScale, m_pianoRollView,
            &CommonGraphicsView::onWheelHorScale);
    auto playbackController = PlaybackController::instance();
    connect(m_timelineView, &TimelineView::setLastPositionTriggered, playbackController,
            [=](double tick) {
                playbackController->setLastPosition(tick);
                playbackController->setPosition(tick);
            });
    connect(appModel, &AppModel::modelChanged, m_timelineView, [=] {
        m_timelineView->setTimeSignature(appModel->timeSignature().numerator,
                                         appModel->timeSignature().denominator);
    });
    connect(appModel, &AppModel::timeSignatureChanged, m_timelineView,
            &TimelineView::setTimeSignature);
    connect(m_pianoRollView, &TimeGraphicsView::timeRangeChanged, m_timelineView,
            &TimelineView::setTimeRange);
    connect(appModel, &AppModel::quantizeChanged, m_timelineView, &TimelineView::setQuantize);

    m_phonemeView = new PhonemeView;
    m_phonemeView->setTimeRange(m_pianoRollView->startTick(), m_pianoRollView->endTick());
    m_phonemeView->setPixelsPerQuarterNote(ClipEditorGlobal::pixelsPerQuarterNote);
    m_phonemeView->setFixedHeight(40);
    m_phonemeView->setVisible(false);
    connect(appModel, &AppModel::modelChanged, m_phonemeView, [=] {
        m_phonemeView->setTimeSignature(appModel->timeSignature().numerator,
                                        appModel->timeSignature().denominator);
    });
    connect(appModel, &AppModel::timeSignatureChanged, m_phonemeView,
            &PhonemeView::setTimeSignature);
    connect(m_pianoRollView, &TimeGraphicsView::timeRangeChanged, m_phonemeView,
            &PhonemeView::setTimeRange);
    connect(appModel, &AppModel::quantizeChanged, m_phonemeView, &PhonemeView::setQuantize);

    connect(playbackController, &PlaybackController::positionChanged, this,
            &ClipEditorView::onPositionChanged);
    connect(playbackController, &PlaybackController::lastPositionChanged, this,
            &ClipEditorView::onLastPositionChanged);

    connect(m_pianoRollView, &PianoRollGraphicsView::removeNoteTriggered, this,
            &ClipEditorView::onRemoveSelectedNotes);
    connect(m_pianoRollView, &PianoRollGraphicsView::editNoteLyricTriggered, this,
            &ClipEditorView::onEditSelectedNotesLyrics);
    connect(m_pianoRollView, &PianoRollGraphicsView::drawNoteCompleted, this,
            &ClipEditorView::onDrawNoteCompleted);
    connect(m_pianoRollView, &PianoRollGraphicsView::moveNotesCompleted, this,
            &ClipEditorView::onMoveNotesCompleted);
    connect(m_pianoRollView, &PianoRollGraphicsView::resizeNoteLeftCompleted, this,
            &ClipEditorView::onResizeNoteLeftCompleted);
    connect(m_pianoRollView, &PianoRollGraphicsView::resizeNoteRightCompleted, this,
            &ClipEditorView::onResizeNoteRightCompleted);

    connect(m_phonemeView, &PhonemeView::adjustCompleted, this,
            &ClipEditorView::onAdjustPhonemeCompleted);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_toolbarView);
    mainLayout->addWidget(m_timelineView);
    mainLayout->addWidget(m_pianoRollView);
    mainLayout->addWidget(m_phonemeView);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins({});
    setLayout(mainLayout);
}
void ClipEditorView::onModelChanged() {
    reset();
}
void ClipEditorView::onSelectedClipChanged(Track *track, Clip *clip) {
    reset();

    if (track == nullptr || clip == nullptr) {
        qDebug() << "ClipEditorView::setIsSingingClip null";
        ClipEditorViewController::instance()->setCurrentSingingClip(nullptr);
        m_toolbarView->setClipName("");
        m_toolbarView->setClipPropertyEditorEnabled(false);
        m_toolbarView->setPianoRollEditToolsEnabled(false);

        m_pianoRollView->setIsSingingClip(false);
        m_pianoRollView->setSceneVisibility(false);

        m_timelineView->setVisible(false);
        m_phonemeView->setVisible(false);
        if (m_track != nullptr) {
            qDebug() << "disconnect track and ClipEditorView";
            disconnect(m_track, &Track::clipChanged, this, &ClipEditorView::onClipChanged);
        }
        if (m_singingClip != nullptr)
            disconnect(m_singingClip, &SingingClip::noteChanged, this,
                       &ClipEditorView::onNoteChanged);
        m_track = nullptr;
        m_clip = nullptr;
        m_singingClip = nullptr;
        return;
    }

    m_track = track;
    m_clip = clip;
    qDebug() << "connect track and ClipEditorView";
    connect(m_track, &Track::clipChanged, this, &ClipEditorView::onClipChanged);
    connect(m_pianoRollView, &TimeGraphicsView::timeRangeChanged, m_timelineView,
            &TimelineView::setTimeRange);
    m_toolbarView->setClipName(clip->name());
    m_toolbarView->setClipPropertyEditorEnabled(true);

    if (clip->type() != Clip::Singing)
        return;
    m_toolbarView->setPianoRollEditToolsEnabled(true);
    m_pianoRollView->setIsSingingClip(true);
    m_pianoRollView->setSceneVisibility(true);
    m_timelineView->setVisible(true);
    m_phonemeView->setVisible(true);
    m_singingClip = dynamic_cast<SingingClip *>(m_clip);
    if (m_singingClip->notes().count() > 0) {
        for (const auto note : m_singingClip->notes()) {
            m_pianoRollView->insertNote(note);
            m_phonemeView->insertNote(note);
        }
        auto firstNote = m_singingClip->notes().at(0);
        qDebug() << "first note start" << firstNote->start();
        m_pianoRollView->setViewportCenterAt(firstNote->start(), firstNote->keyIndex());
    } else
        m_pianoRollView->setViewportCenterAtKeyIndex(60);
    connect(m_singingClip, &SingingClip::noteChanged, this, &ClipEditorView::onNoteChanged);
    ClipEditorViewController::instance()->setCurrentSingingClip(m_singingClip);
}
void ClipEditorView::onClipNameEdited(const QString &name) {
    Clip::ClipCommonProperties args;
    args.name = name;
    args.id = m_clip->id();
    args.start = m_clip->start();
    args.clipStart = m_clip->clipStart();
    args.length = m_clip->length();
    args.clipLen = m_clip->clipLen();
    args.gain = m_clip->gain();
    args.mute = m_clip->mute();
    args.trackIndex = AppModel::instance()->tracks().indexOf(m_track);

    TracksViewController::instance()->onClipPropertyChanged(args);
}
void ClipEditorView::onClipChanged(Track::ClipChangeType type, int id, Clip *clip) {
    if (m_clip == nullptr)
        return;

    if (id == m_clip->id()) {
        if (type == Track::PropertyChanged) {
            onClipPropertyChanged();
        }
    }
}
void ClipEditorView::onEditModeChanged(PianoRollEditMode mode) {
    m_mode = mode;
    m_pianoRollView->setEditMode(m_mode);
}
void ClipEditorView::onPositionChanged(double tick) {
    m_timelineView->setPosition(tick);
    m_pianoRollView->setPlaybackPosition(tick);
    m_phonemeView->setPosition(tick);
}
void ClipEditorView::onLastPositionChanged(double tick) {
    m_pianoRollView->setLastPlaybackPosition(tick);
}
void ClipEditorView::onRemoveSelectedNotes() {
    auto notes = m_pianoRollView->selectedNotesId();
    ClipEditorViewController::instance()->onRemoveNotes(notes);
}
void ClipEditorView::onEditSelectedNotesLyrics() {
    qDebug() << "ClipEditorView::onEditSelectedNotesLyrics";
    auto notes = m_pianoRollView->selectedNotesId();
    ClipEditorViewController::instance()->onEditNotesLyrics(notes);
}
void ClipEditorView::onDrawNoteCompleted(int start, int length, int keyIndex) {
    qDebug() << "ClipEditorView::onDrawNoteCompleted" << start << length << keyIndex;
    auto note = new Note;
    note->setStart(start);
    note->setLength(length);
    note->setKeyIndex(keyIndex);
    note->setLyric(defaultLyric);
    note->setPronunciation(defaultPronunciation);
    ClipEditorViewController::instance()->onInsertNote(note);
}
void ClipEditorView::onMoveNotesCompleted(int deltaTick, int deltaKey) {
    qDebug() << "ClipEditorView::onMoveNotesCompleted"
             << "dt" << deltaTick << "dk" << deltaKey;
    ClipEditorViewController::instance()->onMoveNotes(m_pianoRollView->selectedNotesId(), deltaTick,
                                                      deltaKey);
}
void ClipEditorView::onResizeNoteLeftCompleted(int noteId, int deltaTick) {
    qDebug() << "ClipEditorView::onResizeNoteLeftCompleted"
             << "id" << noteId << "dt" << deltaTick;
    QList<int> notes;
    notes.append(noteId);
    ClipEditorViewController::instance()->onResizeNotesLeft(notes, deltaTick);
}
void ClipEditorView::onResizeNoteRightCompleted(int noteId, int deltaTick) {
    qDebug() << "ClipEditorView::onResizeNoteRightCompleted"
             << "id" << noteId << "dt" << deltaTick;
    QList<int> notes;
    notes.append(noteId);
    ClipEditorViewController::instance()->onResizeNotesRight(notes, deltaTick);
}
void ClipEditorView::onAdjustPhonemeCompleted(PhonemeView::PhonemeViewModel *phonemeViewModel) {
    if (!phonemeViewModel)
        return;

    auto appModel = AppModel::instance();
    QList<int> notesId;
    QList<Phoneme> phonemes;
    notesId.append(phonemeViewModel->noteId);
    auto note = m_singingClip->findNoteById(phonemeViewModel->noteId);
    Phoneme phoneme;
    phoneme.name = phonemeViewModel->name;
    auto noteStartInMs = appModel->tickToMs(note->start());
    auto phonemeViewModelStartInMs =
        appModel->tickToMs(phonemeViewModel->start + phonemeViewModel->startOffset);
    if (phonemeViewModel->type == PhonemeView::PhonemeViewModel::Ahead) {
        phoneme.type = Phoneme::Ahead;
        phoneme.start = qRound(noteStartInMs - phonemeViewModelStartInMs);
        qDebug() << "ClipEditorView::onAdjustPhonemeCompleted"
                 << "append ahead" << phoneme.name << phoneme.start;
    } else if (phonemeViewModel->type == PhonemeView::PhonemeViewModel::Normal) {
        phoneme.type = Phoneme::Normal;
        phoneme.start = qRound(phonemeViewModelStartInMs - noteStartInMs);
        qDebug() << "ClipEditorView::onAdjustPhonemeCompleted"
                 << "append normal" << phoneme.name << phoneme.start;
    }
    phonemes.append(phoneme);
    ClipEditorViewController::instance()->onAdjustPhoneme(notesId, phonemes);
}
void ClipEditorView::reset() {
    m_pianoRollView->reset();
    m_phonemeView->reset();
}
void ClipEditorView::onClipPropertyChanged() {
    qDebug() << "ClipEditorView::handleClipPropertyChange" << m_clip->id() << m_clip->start();
    reset();
    auto singingClip = dynamic_cast<SingingClip *>(m_clip);
    if (singingClip->notes().count() <= 0)
        return;
    for (const auto note : singingClip->notes()) {
        m_pianoRollView->insertNote(note);
        m_phonemeView->insertNote(note);
    }
}
void ClipEditorView::onNoteChanged(SingingClip::NoteChangeType type, int id, Note *note) {
    switch (type) {
        case SingingClip::Inserted:
            m_pianoRollView->insertNote(note);
            m_phonemeView->insertNote(note);
            break;
        case SingingClip::PropertyChanged:
            m_pianoRollView->updateNote(note);
            m_phonemeView->updateNote(note);
            break;
        case SingingClip::Removed:
            m_pianoRollView->removeNote(id);
            m_phonemeView->removeNote(id);
            break;
    }
    m_pianoRollView->updateOverlappedState(m_singingClip);
}