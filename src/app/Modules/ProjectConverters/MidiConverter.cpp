#include "MidiConverter.h"

#include "Model/AppModel/AudioClip.h"
#include "opendspx/qdspxtrack.h"
#include "opendspx/qdspxtimeline.h"
#include "opendspx/qdspxmodel.h"
#include "opendspx/converters/midi.h"

#include <QMessageBox>
#include <QTextCodec>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDialog>
#include <QListWidget>

#include "Model/AppModel/Track.h"

#include "Model/AppModel/Note.h"
#include "Model/AppModel/SingingClip.h"
#include "UI/Controls/ComboBox.h"

#include <QTextEdit>

QWidget *createPreviewTab(const QList<QDspx::MidiConverter::TrackInfo> &trackInfoList,
                          QTextCodec **codec) {
    // Create a widget for the tab
    auto *tab = new QWidget();

    // Create a layout for the tab
    auto *tabLayout = new QVBoxLayout(tab);

    // Create a horizontal layout for the lists and preview
    auto *listLayout = new QHBoxLayout();

    // Create a list widget to show track names
    auto *trackListWidget = new QListWidget();
    for (const auto &trackInfo : trackInfoList) {
        auto *item = new QListWidgetItem(QString(trackInfo.title.constData()));
        trackListWidget->addItem(item);
    }
    listLayout->addWidget(trackListWidget);

    // Create a combo box for selecting common encodings
    auto *encodingComboBox = new ComboBox();
    encodingComboBox->addItem("UTF-8");
    encodingComboBox->addItem("ISO 8859-1");
    encodingComboBox->addItem("GBK");
    encodingComboBox->addItem("Shift-JIS");
    // Add more encodings as needed
    listLayout->addWidget(encodingComboBox);

    // Create a text edit for previewing lyrics
    auto *lyricsPreview = new QTextEdit();
    lyricsPreview->setReadOnly(true);
    listLayout->addWidget(lyricsPreview);

    // Add the lists and preview to the tab layout
    tabLayout->addLayout(listLayout);

    // Connect the track list selection to update the preview
    QObject::connect(trackListWidget, &QListWidget::currentRowChanged, [=](int currentRow) {
        if (currentRow >= 0 && currentRow < trackInfoList.size()) {
            QByteArray lyricsData;
            for (const auto &clip : trackInfoList[currentRow].lyrics)
                lyricsData += clip;
            const QTextCodec *selectedCodec =
                QTextCodec::codecForName(encodingComboBox->currentText().toUtf8());
            const QString decodedLyrics = selectedCodec->toUnicode(lyricsData);
            lyricsPreview->setPlainText(decodedLyrics);
        }
    });

    // Connect the encoding selection to update the preview
    QObject::connect(encodingComboBox, &QComboBox::currentTextChanged,
                     [=](const QString &codecName) {
                         const int currentRow = trackListWidget->currentRow();
                         if (currentRow >= 0 && currentRow < trackInfoList.size()) {
                             QByteArray lyricsData;
                             for (const auto &lyric : trackInfoList[currentRow].lyrics)
                                 lyricsData += lyric + " ";
                             if (QTextCodec::codecForName(codecName.toUtf8()) == nullptr) {
                                 qWarning() << "Codec not found for" << codecName;
                             } else {
                                 *codec = QTextCodec::codecForName(codecName.toUtf8());
                                 const QString decodedLyrics = (*codec)->toUnicode(lyricsData);
                                 lyricsPreview->setPlainText(decodedLyrics);
                             }
                         }
                     });
    return tab;
}

bool trackSelector(const QList<QDspx::MidiConverter::TrackInfo> &trackInfoList,
                   const QList<QByteArray> &labelList, QList<int> *selectIDs, QTextCodec **codec) {
    // Create a dialog
    QDialog dialog;
    dialog.setWindowTitle("MIDI Track Selector");

    // Create a layout for the dialog
    auto *layout = new QVBoxLayout(&dialog);

    // Create a QTabWidget to hold multiple tabs
    auto *tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);

    // Create the original checkboxes tab
    auto *checkBoxTab = new QWidget();
    auto *checkBoxLayout = new QVBoxLayout(checkBoxTab);

    // Create checkboxes for each MIDI track
    QList<QCheckBox *> checkBoxes;
    for (const auto &trackInfo : trackInfoList) {
        auto *checkBox = new QCheckBox(QString("track %1:  %2 notes (%3)")
                                           .arg(trackInfo.title.constData())
                                           .arg(trackInfo.lyrics.count())
                                           .arg(trackInfo.keyRange));
        checkBox->setChecked(false);
        checkBoxes.append(checkBox);
        checkBoxLayout->addWidget(checkBox);
    }

    tabWidget->addTab(checkBoxTab, "Track Selection");

    QTextCodec *newCodec = QTextCodec::codecForName("UTF-8");
    const auto previewTab = createPreviewTab(trackInfoList, &newCodec);

    // Add the preview tab using the separate function
    tabWidget->addTab(previewTab, "Track Preview");

    // Create OK and Cancel buttons
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    // Connect the button signals to slots on the dialog
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Execute the dialog and get the result
    const int result = dialog.exec();

    // Process the selected MIDI tracks
    if (result == QDialog::Accepted) {
        selectIDs->clear();
        for (int i = 0; i < checkBoxes.size(); ++i) {
            if (checkBoxes.at(i)->isChecked()) {
                selectIDs->append(i);
            }
        }
        if (newCodec != nullptr)
            *codec = newCodec;
        return true;
    } else {
        // User canceled the dialog
        return false;
    }
}

int MidiConverter::midiImportHandler() {
    QMessageBox msgBox;
    msgBox.setText("MIDI Import");
    msgBox.setInformativeText("Do you want to create a new track or use a new project?");
    const QPushButton *newTrackButton = msgBox.addButton("New Track", QMessageBox::ActionRole);
    const QPushButton *newProjectButton = msgBox.addButton("New Project", QMessageBox::ActionRole);
    msgBox.addButton("Cancel", QMessageBox::RejectRole);
    msgBox.exec();

    QAbstractButton *clickedButton = msgBox.clickedButton();
    const auto *clickedPushButton = qobject_cast<QPushButton *>(clickedButton);
    if (clickedPushButton == newTrackButton) {
        return ImportMode::AppendToProject;
    } else if (clickedPushButton == newProjectButton) {
        return ImportMode::NewProject;
    } else {
        return -1;
    }
}

bool midiOverlapHandler() {
    QMessageBox msgBox;
    msgBox.setText("MIDI Overlap");
    msgBox.setInformativeText("The MIDI file contains overlapping notes. Do you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        return true;
    } else {
        return false;
    }
}

bool MidiConverter::load(const QString &path, AppModel *model, QString &errMsg, ImportMode mode) {
    const auto dspx = new QDspx::Model;
    std::function<bool(const QList<QDspx::MidiConverter::TrackInfo> &, const QList<QByteArray> &,
                       QList<int> *, QTextCodec **)>
        midiSelector = trackSelector;
    QVariantMap args = {};
    args.insert(QStringLiteral("selector"),
                QVariant::fromValue(reinterpret_cast<quintptr>(&midiSelector)));

    const auto midi = new QDspx::MidiConverter();

    auto decodeNotes = [](const QList<QDspx::Note> &arrNotes) {
        QList<Note *> notes;
        for (const QDspx::Note &dsNote : arrNotes) {
            auto note = new Note;
            note->setStart(dsNote.pos);
            note->setLength(dsNote.length);
            note->setKeyIndex(dsNote.keyNum);
            note->setLyric(dsNote.lyric);
            notes.append(note);
        }
        return notes;
    };

    auto decodeClips = [&](const QDspx::Track &track, Track *dsTack) {
        for (auto &clip : track.clips) {
            if (clip->type == QDspx::Clip::Type::Singing) {
                auto singClip = clip.dynamicCast<QDspx::SingingClip>();
                auto singingClip = new SingingClip;
                singingClip->setName(clip->name);
                singingClip->setStart(clip->time.start);
                singingClip->setClipStart(clip->time.clipStart);
                singingClip->setLength(clip->time.length);
                singingClip->setClipLen(clip->time.clipLen);
                auto notes = decodeNotes(singClip->notes);
                for (auto &note : notes)
                    singingClip->insertNote(note);
                dsTack->insertClip(singingClip);
            } else if (clip->type == QDspx::Clip::Type::Audio) {
                auto audioClip = new AudioClip;
                audioClip->setName(clip->name);
                audioClip->setStart(clip->time.start);
                audioClip->setClipStart(clip->time.clipStart);
                audioClip->setLength(clip->time.length);
                audioClip->setClipLen(clip->time.clipLen);
                audioClip->setPath(clip.dynamicCast<QDspx::AudioClip>()->path);
                dsTack->insertClip(audioClip);
            }
        }
    };

    auto decodeTracks = [&](const QDspx::Model *_dspx, AppModel *_model, ImportMode _mode) {
        for (int i = 0; i < _dspx->content.tracks.count(); i++) {
            auto track = _dspx->content.tracks[i];
            const auto dsTrack = new Track;
            dsTrack->setName(track.name);
            decodeClips(track, dsTrack);
            _model->insertTrack(dsTrack, i);
        }
    };

    const auto returnCode = midi->load(path, dspx, args);

    if (returnCode.type != QDspx::Result::Success) {
        QMessageBox::warning(nullptr, "Warning",
                             QString("Failed to load midi file.\r\npath: %1\r\ntype: %2 code: %3")
                                 .arg(path)
                                 .arg(returnCode.type)
                                 .arg(returnCode.code));
        return false;
    }

    if (mode == ImportMode::NewProject) {
        model->newProject();
        model->clearTracks();
    } else if (mode == ImportMode::AppendToProject) {
        if (model->timeSignature().numerator != dspx->content.timeline.timeSignatures[0].num ||
            model->timeSignature().denominator != dspx->content.timeline.timeSignatures[0].den) {
            QMessageBox msgBox;
            msgBox.setText("Time Signature Mismatch");
            msgBox.setInformativeText("The time signature of the MIDI file does not match the "
                                      "current project. Do you want to continue?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret == QMessageBox::No) {
                return false;
            }
        } else if (model->tempo() != dspx->content.timeline.tempos[0].value) {
            QMessageBox msgBox;
            msgBox.setText("Tempo Mismatch");
            msgBox.setInformativeText("The tempo of the MIDI file does not match the current "
                                      "project. Do you want to continue?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret == QMessageBox::No) {
                return false;
            }
        }
    } else {
        return false;
    }

    if (dspx->content.tracks.count() > 0) {
        auto timeline = dspx->content.timeline;
        model->setTimeSignature(
            TimeSignature(timeline.timeSignatures[0].num, timeline.timeSignatures[0].den));
        model->setTempo(timeline.tempos[0].value);
        decodeTracks(dspx, model, mode);
        return true;
    }
    return false;
}

bool MidiConverter::save(const QString &path, AppModel *model, QString &errMsg) {
    QDspx::Model dspx;
    std::function<bool()> midiOverlap = midiOverlapHandler;
    QVariantMap args = {};
    args.insert(QStringLiteral("overlapHandler"),
                QVariant::fromValue(reinterpret_cast<quintptr>(&midiOverlap)));
    auto midi = new QDspx::MidiConverter;

    auto encodeNotes = [](const OverlappableSerialList<Note> &notes) {
        QList<QDspx::Note> arrNotes;
        for (const auto &note : notes) {
            QDspx::Note dsNote;
            dsNote.pos = note->start();
            dsNote.length = note->length();
            dsNote.keyNum = note->keyIndex();
            dsNote.lyric = note->lyric();
            arrNotes.append(dsNote);
        }
        return arrNotes;
    };

    auto encodeClips = [&](const Track *dsTrack, QDspx::Track *track) {
        for (const auto &clip : dsTrack->clips()) {
            if (clip->clipType() == Clip::Singing) {
                auto singingClip = dynamic_cast<SingingClip *>(clip);
                auto singClip = QDspx::SingingClipRef::create();
                singClip->name = clip->name();
                singClip->time.start = clip->start();
                singClip->time.clipStart = clip->clipStart();
                singClip->time.length = clip->length();
                singClip->time.clipLen = clip->clipLen();
                singClip->notes = encodeNotes(singingClip->notes());
                track->clips.append(singClip);
            } else if (clip->clipType() == Clip::Audio) {
                auto audioClip = dynamic_cast<AudioClip *>(clip);
                auto audioClipRef = QDspx::AudioClipRef::create();
                audioClipRef->name = clip->name();
                audioClipRef->time.start = clip->start();
                audioClipRef->time.clipStart = clip->clipStart();
                audioClipRef->time.length = clip->length();
                audioClipRef->time.clipLen = clip->clipLen();
                audioClipRef->path = audioClip->path();
                track->clips.append(audioClipRef);
            }
        }
    };

    auto encodeTracks = [&](const AppModel *_model, QDspx::Model &_dspx) {
        for (const auto &dsTrack : _model->tracks()) {
            QDspx::Track track;
            track.name = dsTrack->name();
            encodeClips(dsTrack, &track);
            _dspx.content.tracks.append(track);
        }
    };

    auto timeline = new QDspx::Timeline;

    timeline->tempos.append(QDspx::Tempo(0, model->tempo()));
    timeline->timeSignatures.append(QDspx::TimeSignature(0, model->timeSignature().numerator,
                                                         model->timeSignature().denominator));
    dspx.content.timeline = *timeline;

    encodeTracks(model, dspx);
    auto returnCode = midi->save(path, dspx, args);

    if (returnCode.type != QDspx::Result::Success) {
        QMessageBox::warning(nullptr, "Warning",
                             QString("Failed to save midi file.\r\npath: %1\r\ntype: %2 code: %3")
                                 .arg(path)
                                 .arg(returnCode.type)
                                 .arg(returnCode.code));
        return false;
    }
    return true;
}
