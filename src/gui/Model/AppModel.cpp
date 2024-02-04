//
// Created by fluty on 2024/1/27.
//

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QTextCodec>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>

#include "AppModel.h"

#include "Utils/IdGenerator.h"
#include "opendspx/qdspxtrack.h"
#include "opendspx/qdspxmodel.h"
#include "opendspx/converters/midi.h"

int AppModel::numerator() const {
    return m_numerator;
}
void AppModel::setNumerator(int numerator) {
    m_numerator = numerator;
    emit timeSignatureChanged(m_numerator, m_denominator);
}
int AppModel::denominator() const {
    return m_denominator;
}
void AppModel::setDenominator(int denominator) {
    m_denominator = denominator;
    emit timeSignatureChanged(m_numerator, m_denominator);
}
double AppModel::tempo() const {
    return m_tempo;
}
void AppModel::setTempo(double tempo) {
    m_tempo = tempo;
    emit tempoChanged(m_tempo);
}
const QList<DsTrack *> &AppModel::tracks() const {
    return m_tracks;
}

void AppModel::insertTrack(DsTrack *track, int index) {
    // connect(track, &DsTrack::clipChanged, this, [=] {
    //     auto trackIndex = m_tracks.indexOf(track);
    //     emit tracksChanged(PropertyUpdate, trackIndex);
    // });
    connect(track, &DsTrack::propertyChanged, this, [=] {
        auto trackIndex = m_tracks.indexOf(track);
        emit tracksChanged(PropertyUpdate, trackIndex);
    });
    m_tracks.insert(index, track);
    emit tracksChanged(Insert, index);
}
void AppModel::removeTrack(int index) {
    m_tracks.removeAt(index);
    emit tracksChanged(Remove, index);
}

bool trackSelector(const QList<QDspx::MidiConverter::TrackInfo> &trackInfoList,
                   const QList<QByteArray> &labelList, QList<int> *selectIDs, QTextCodec *codec) {

    // Set UTF-8 as the text codec
    codec = QTextCodec::codecForName("UTF-8");

    // Create a dialog
    QDialog dialog;
    dialog.setWindowTitle("MIDI Track Selector");

    // Create a layout for the dialog
    auto *layout = new QVBoxLayout(&dialog);

    // Create checkboxes for each MIDI track
    QList<QCheckBox *> checkBoxes;
    for (const auto &trackInfo : trackInfoList) {
        auto *checkBox = new QCheckBox(QString("track %1:  %2 notes")
                                           .arg(trackInfo.title.constData())
                                           .arg(trackInfo.lyrics.count()));
        checkBox->setChecked(false);
        checkBoxes.append(checkBox);
        layout->addWidget(checkBox);
    }

    // Create OK and Cancel buttons
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    // Connect the button signals to slots on the dialog
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Execute the dialog and get the result
    int result = dialog.exec();

    // Process the selected MIDI tracks
    if (result == QDialog::Accepted) {
        selectIDs->clear();
        for (int i = 0; i < checkBoxes.size(); ++i) {
            if (checkBoxes.at(i)->isChecked()) {
                selectIDs->append(i);
            }
        }
        return true;
    } else {
        // User canceled the dialog
        return false;
    }
}

bool AppModel::importMidi(const QString &filename) {
    reset();

    auto dspx = new QDspx::Model;
    std::function<bool(const QList<QDspx::MidiConverter::TrackInfo> &, const QList<QByteArray> &,
                       QList<int> *, QTextCodec *)>
        midiSelector = trackSelector;
    QVariantMap args = {};
    args.insert(QStringLiteral("selector"),
                QVariant::fromValue(reinterpret_cast<quintptr>(&midiSelector)));

    auto midi = new QDspx::MidiConverter;

    auto decodeNotes = [](const QList<QDspx::Note> &arrNotes) {
        QList<DsNote> notes;
        for (const QDspx::Note &dsNote : arrNotes) {
            DsNote note;
            note.setStart(dsNote.pos);
            note.setLength(dsNote.length);
            note.setKeyIndex(dsNote.keyNum);
            note.setLyric(dsNote.lyric);
            note.setPronunciation("la");
            notes.append(note);
        }
        return notes;
    };

    auto decodeClips = [&](const QDspx::Track &track, DsTrack *dsTack) {
        for (auto &clip : track.clips) {
            if (clip->type == QDspx::Clip::Type::Singing) {
                auto singClip = clip.dynamicCast<QDspx::SingingClip>();
                auto singingClip = new DsSingingClip;
                singingClip->setName(clip->name);
                singingClip->setStart(clip->time.start);
                singingClip->setClipStart(clip->time.clipStart);
                singingClip->setLength(clip->time.length);
                singingClip->setClipLen(clip->time.clipLen);
                singingClip->notes.append(decodeNotes(singClip->notes));
                dsTack->insertClip(singingClip);
            } else if (clip->type == QDspx::Clip::Type::Audio) {
                auto audioClip = new DsAudioClip;
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

    auto decodeTracks = [&](const QDspx::Model *dspx, QList<DsTrack *> &tracks) {
        for (int i = 0; i < dspx->content.tracks.count(); i++) {
            auto track = dspx->content.tracks[i];
            auto dsTrack = new DsTrack;
            dsTrack->setName(track.name);
            decodeClips(track, dsTrack);
            tracks.append(dsTrack);
        }
    };

    auto returnCode = midi->load(filename, dspx, args);

    if (returnCode.type != QDspx::ReturnCode::Success) {
        QMessageBox::warning(nullptr, "Warning",
                             QString("Failed to load midi file.\r\npath: %1\r\ntype: %2 code: %3")
                                 .arg(filename)
                                 .arg(returnCode.type)
                                 .arg(returnCode.code));
        return false;
    }

    auto timeline = dspx->content.timeline;
    m_numerator = timeline.timeSignatures[0].num;
    m_denominator = timeline.timeSignatures[0].den;
    m_tempo = timeline.tempos[0].value;
    decodeTracks(dspx, m_tracks);
    emit modelChanged();
    return true;
}

bool AppModel::loadAProject(const QString &filename) {
    reset();

    auto openJsonFile = [](const QString &filename, QJsonObject *jsonObj) {
        QFile loadFile(filename);
        if (!loadFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open project file";
            return false;
        }
        QByteArray allData = loadFile.readAll();
        loadFile.close();
        QJsonParseError err;
        QJsonDocument json = QJsonDocument::fromJson(allData, &err);
        if (err.error != QJsonParseError::NoError)
            return false;
        if (json.isObject()) {
            *jsonObj = json.object();
        }
        return true;
    };

    auto decodeNotes = [](const QJsonArray &arrNotes) {
        QList<DsNote> notes;
        for (const auto valNote : qAsConst(arrNotes)) {
            auto objNote = valNote.toObject();
            DsNote note;
            note.setStart(objNote.value("pos").toInt());
            note.setLength(objNote.value("dur").toInt());
            note.setKeyIndex(objNote.value("pitch").toInt());
            note.setLyric(objNote.value("lyric").toString());
            note.setPronunciation("la");
            notes.append(note);
        }
        return notes;
    };

    auto decodeClips = [&](const QJsonArray &arrClips, DsTrack *dsTack, const QString &type,
                           int trackIndex) {
        for (const auto &valClip : qAsConst(arrClips)) {
            auto objClip = valClip.toObject();
            if (type == "sing") {
                auto singingClip = new DsSingingClip;
                singingClip->setName("Clip");
                singingClip->setStart(objClip.value("pos").toInt());
                singingClip->setClipStart(objClip.value("clipPos").toInt());
                singingClip->setLength(objClip.value("dur").toInt());
                singingClip->setClipLen(objClip.value("clipDur").toInt());
                auto arrNotes = objClip.value("notes").toArray();
                singingClip->notes.append(decodeNotes(arrNotes));
                dsTack->insertClip(singingClip);
            } else if (type == "audio") {
                auto audioClip = new DsAudioClip;
                audioClip->setName("Clip");
                audioClip->setStart(objClip.value("pos").toInt());
                audioClip->setClipStart(objClip.value("clipPos").toInt());
                audioClip->setLength(objClip.value("dur").toInt());
                audioClip->setClipLen(objClip.value("clipDur").toInt());
                audioClip->setPath(objClip.value("path").toString());
                dsTack->insertClip(audioClip);
            }
        }
    };

    auto decodeTracks = [&](const QJsonArray &arrTracks, QList<DsTrack *> &tracks) {
        int i = 0;
        for (const auto &valTrack : qAsConst(arrTracks)) {
            auto objTrack = valTrack.toObject();
            auto type = objTrack.value("type").toString();
            auto track = new DsTrack;
            decodeClips(objTrack.value("patterns").toArray(), track, type, i);
            tracks.append(track);
            i++;
        }
    };

    QJsonObject objAProject;
    if (openJsonFile(filename, &objAProject)) {
        m_numerator = objAProject.value("beatsPerBar").toInt();
        m_tempo = objAProject.value("tempos").toArray().first().toObject().value("bpm").toDouble();
        decodeTracks(objAProject.value("tracks").toArray(), m_tracks);
        // auto clip = tracks().first().clips.first().dynamicCast<DsSingingClip>();
        // qDebug() << clip->notes.count();
        emit modelChanged();
        return true;
    }
    return false;
}
void AppModel::onTrackUpdated(int index) {
    emit tracksChanged(PropertyUpdate, index);
}
void AppModel::onSelectedClipChanged(int trackIndex, int clipId) {
    m_selectedClipTrackIndex = trackIndex;
    m_selectedClipId = clipId;
    emit selectedClipChanged(m_selectedClipTrackIndex, m_selectedClipId);
}
void AppModel::reset() {
    m_tracks.clear();
}