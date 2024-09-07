//
// Created by fluty on 2024/1/27.
//

#ifndef DSNOTE_H
#define DSNOTE_H

#include "Utils/Overlappable.h"
#include "Utils/UniqueObject.h"
#include "PhonemeInfo.h"
#include "Pronunciation.h"

#include <QObject>

class SingingClip;

class Note : public QObject, public Overlappable, public UniqueObject {
    Q_OBJECT

public:
    enum WordPropertyType { Original, Edited };
    explicit Note(SingingClip *context = nullptr, QObject *parent = nullptr);
    ~Note() override;
    [[nodiscard]] SingingClip *clip() const;
    void setClip(SingingClip *clip);
    [[nodiscard]] int start() const;
    void setStart(int start);
    [[nodiscard]] int rStart() const;
    void setRStart(int rStart);
    [[nodiscard]] int length() const;
    void setLength(int length);
    [[nodiscard]] int keyIndex() const;
    void setKeyIndex(int keyIndex);
    [[nodiscard]] QString lyric() const;
    void setLyric(const QString &lyric);
    [[nodiscard]] Pronunciation pronunciation() const;
    void setPronunciation(const Pronunciation &pronunciation);
    void setPronunciation(WordPropertyType type, const QString &text);
    [[nodiscard]] QStringList pronCandidates() const;
    void setPronCandidates(const QStringList &pronCandidates);
    [[nodiscard]] PhonemeInfo phonemeInfo() const;
    void setPhonemeInfo(WordPropertyType type, const QList<Phoneme> &phonemes);
    void setPhonemeInfo(const QList<Phoneme> &original, const QList<Phoneme> &edited);
    [[nodiscard]] QString language() const;
    void setLanguage(const QString &language);
    [[nodiscard]] bool lineFeed() const;
    void setLineFeed(const bool &lineFeed);
    [[nodiscard]] bool isSlur() const;

    int compareTo(const Note *obj) const;
    bool isOverlappedWith(Note *obj) const;
    [[nodiscard]] std::tuple<qsizetype, qsizetype> interval() const override;

    friend QDataStream &operator<<(QDataStream &out, const Note &note);
    friend QDataStream &operator>>(QDataStream &in, Note &note);

    // static QJsonObject serialize(const Note &note);
    // static Note deserialize(const QJsonObject &objNote);

    class WordProperties {
    public:
        QString lyric;
        QString language;
        Pronunciation pronunciation;
        QStringList pronCandidates;
        PhonemeInfo phonemes;

        static WordProperties fromNote(const Note &note);
    };

private:
    SingingClip *m_clip = nullptr;
    // int m_start = 0;
    int m_rStart = 0;
    int m_length = 480;
    int m_keyIndex = 60;
    QString m_lyric;
    QString m_language = "unknown";
    Pronunciation m_pronunciation;
    QStringList m_pronCandidates;
    PhonemeInfo m_phonemes;
    bool m_lineFeed = false;
};

#endif // DSNOTE_H