//
// Created by fluty on 2024/1/27.
//

#include "Note.h"

#include "Clip.h"

#include <QJsonArray>

Note::Note(SingingClip *context, QObject *parent) : QObject(parent), m_clip(context) {
}

Note::~Note() {
    // qDebug() << "~Note()" << id() << m_lyric;
}

SingingClip *Note::clip() const {
    return m_clip;
}

void Note::setClip(SingingClip *clip) {
    m_clip = clip;
}

int Note::start() const {
    if (!m_clip) {
        qFatal() << "SingingClip is null";
        return 0;
    }
    auto offset = m_clip->start();
    return m_rStart + offset;
}

void Note::setStart(int start) {
    if (!m_clip) {
        qFatal() << "SingingClip is null";
        return;
    }
    auto offset = m_clip->start();
    auto rStart = start - offset;
    Q_ASSERT(rStart >= 0);
    m_rStart = rStart;
}

int Note::rStart() const {
    return m_rStart;
}

void Note::setRStart(int rStart) {
    Q_ASSERT(rStart >= 0);
    m_rStart = rStart;
}

int Note::length() const {
    return m_length;
}

void Note::setLength(int length) {
    Q_ASSERT(length >= 0);
    m_length = length;
}

int Note::keyIndex() const {
    return m_keyIndex;
}

void Note::setKeyIndex(int keyIndex) {
    Q_ASSERT(keyIndex >= 0 && keyIndex <= 127);
    m_keyIndex = keyIndex;
}

QString Note::lyric() const {
    return m_lyric;
}

void Note::setLyric(const QString &lyric) {
    m_lyric = lyric;
}

Pronunciation Note::pronunciation() const {
    return m_pronunciation;
}

void Note::setPronunciation(const Pronunciation &pronunciation) {
    m_pronunciation = pronunciation;
}

void Note::setPronunciation(WordPropertyType type, const QString &text) {
    if (type == Original)
        m_pronunciation.original = text;
    else if (type == Edited)
        m_pronunciation.edited = text;
}

QStringList Note::pronCandidates() const {
    return m_pronCandidates;
}

void Note::setPronCandidates(const QStringList &pronCandidates) {
    m_pronCandidates = pronCandidates;
}

PhonemeInfo Note::phonemeInfo() const {
    return m_phonemes;
}

void Note::setPhonemeInfo(WordPropertyType type, const QList<Phoneme> &phonemes) {
    if (type == Original)
        m_phonemes.original = phonemes;
    else if (type == Edited)
        m_phonemes.edited = phonemes;
}

void Note::setPhonemeInfo(const QList<Phoneme> &original, const QList<Phoneme> &edited) {
    m_phonemes.original = original;
    m_phonemes.edited = edited;
}

QString Note::language() const {
    return m_language;
}

void Note::setLanguage(const QString &language) {
    m_language = language;
}

bool Note::lineFeed() const {
    return m_lineFeed;
}

void Note::setLineFeed(const bool &lineFeed) {
    m_lineFeed = lineFeed;
}

bool Note::isSlur() const {
    return m_lyric.contains('-');
}

int Note::compareTo(const Note *obj) const {
    if (!m_clip) {
        qFatal() << "SingingClip is null";
        return 0;
    }
    if (m_clip != obj->m_clip) {
        qFatal() << "SingingClip is not the same";
        return 0;
    }
    const auto otherStart = obj->rStart();
    if (rStart() < otherStart)
        return -1;
    if (rStart() > otherStart)
        return 1;
    return 0;
}

std::tuple<qsizetype, qsizetype> Note::interval() const {
    return std::make_tuple(rStart(), rStart() + length());
}

Note::WordProperties Note::WordProperties::fromNote(const Note &note) {
    WordProperties properties;
    properties.lyric = note.lyric();
    properties.pronunciation = note.pronunciation();
    properties.language = note.language();
    properties.pronCandidates = note.pronCandidates();
    properties.phonemes = note.phonemeInfo();
    return properties;
}

QDataStream &operator<<(QDataStream &out, const Note &note) {
    // out << note.m_id;
    out << note.m_rStart;
    out << note.m_length;
    out << note.m_keyIndex;
    out << note.m_lyric;
    out << note.m_language;
    out << note.m_pronunciation;
    out << note.m_phonemes;
    return out;
}

QDataStream &operator>>(QDataStream &in, Note &note) {
    in >> note.m_rStart;
    in >> note.m_length;
    in >> note.m_keyIndex;
    in >> note.m_lyric;
    in >> note.m_language;
    in >> note.m_pronunciation;
    in >> note.m_phonemes;
    return in;
}