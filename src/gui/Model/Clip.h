//
// Created by fluty on 2024/1/27.
//

#ifndef DSCLIP_H
#define DSCLIP_H

#include <QSharedPointer>
#include <QObject>

#include "Note.h"
#include "Params.h"

class Clip : public QObject, public IOverlapable, public UniqueObject {
    Q_OBJECT

public:
    enum ClipType { Audio, Singing, Generic };

    virtual ~Clip() = default;

    virtual ClipType type() const {
        return Generic;
    }
    QString name() const;
    void setName(const QString &text);
    int start() const;
    void setStart(int start);
    int length() const;
    void setLength(int length);
    int clipStart() const;
    void setClipStart(int clipStart);
    int clipLen() const;
    void setClipLen(int clipLen);
    double gain() const;
    void setGain(double gain);
    bool mute() const;
    void setMute(bool mute);

    int compareTo(Clip *obj) const;
    bool isOverlappedWith(Clip *obj) const;

    class ClipCommonProperties {
    public:
        virtual ~ClipCommonProperties() = default;
        int id = -1;

        QString name;
        int start = 0;
        int length = 0;
        int clipStart = 0;
        int clipLen = 0;
        double gain = 0;
        bool mute = false;

        int trackIndex = 0;
    };
    class AudioClipPropertyChangedArgs : public ClipCommonProperties {
    public:
        QString path;
    };

protected:
    QString m_name;
    int m_start = 0;
    int m_length = 0;
    int m_clipStart = 0;
    int m_clipLen = 0;
    double m_gain = 0;
    bool m_mute = false;
};

class DsAudioClip final : public Clip {
public:
    ClipType type() const override {
        return Audio;
    }
    QString path() const {
        return m_path;
    }
    void setPath(const QString &path) {
        m_path = path;
    }

private:
    QString m_path;
};

class DsSingingClip final : public Clip {
    Q_OBJECT

public:
    enum NoteChangeType { Inserted, PropertyChanged, Removed };
    enum ParamsChangeType { Pitch, Energy, Tension, Breathiness };

    ClipType type() const override {
        return Singing;
    }

    const OverlapableSerialList<Note> &notes() const;
    void insertNote(Note *note);
    void removeNote(Note *note);
    void insertNoteQuietly(Note *note);
    void removeNoteQuietly(Note *note);
    void notifyNotePropertyChanged(Note *note);
    Note *findNoteById(int id);

    Params params;
    // const DsParams &params() const;

signals:
    void noteChanged(NoteChangeType type, int id, Note *note);
    void paramsChanged(ParamsChangeType type);

private:
    OverlapableSerialList<Note> m_notes;
    // DsParams m_params;
};

// using DsClipPtr = QSharedPointer<DsClip>;
// using DsSingingClipPtr = QSharedPointer<DsSingingClip>;
// using DsAudioClipPtr = QSharedPointer<DsAudioClip>;

#endif // DSCLIP_H
