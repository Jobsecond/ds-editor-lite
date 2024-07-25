//
// Created by fluty on 2024/1/22.
//

#ifndef SINGINGCLIPGRAPHICSITEM_H
#define SINGINGCLIPGRAPHICSITEM_H

#include "AbstractClipGraphicsItem.h"
#include "Model/Clip.h"
#include "Utils/OverlappableSerialList.h"

class Note;

class SingingClipGraphicsItem final : public AbstractClipGraphicsItem {
    Q_OBJECT
public:
    class NoteViewModel : public IOverlapable {
    public:
        int id{};
        int rStart{};
        int length{};
        int keyIndex{};

        int compareTo(NoteViewModel *obj) const;
        static bool isOverlappedWith(NoteViewModel *obj) ;
    };

    [[nodiscard]] ClipType clipType() const override {
        return Singing;
    }
    explicit SingingClipGraphicsItem(int itemId, QGraphicsItem *parent = nullptr);
    ~SingingClipGraphicsItem() override = default;

    void loadNotes(const OverlappableSerialList<Note> &notes);
    [[nodiscard]] QString audioCachePath() const;
    void setAudioCachePath(const QString &path);

public slots:
    void onNoteListChanged(SingingClip::NoteChangeType type, Note *note);
    void onNotePropertyChanged(Note::NotePropertyType type, Note *note);

private:
    // void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    // override;
    void drawPreviewArea(QPainter *painter, const QRectF &previewRect, int opacity) override;
    QString clipTypeName() override {
        return "[Singing] ";
    }

    QString m_audioCachePath;
    OverlappableSerialList<NoteViewModel> m_notes;

    void addNote(Note *note);
    void removeNote(int id);
    // void updateNote(Note *note);
};



#endif // SINGINGCLIPGRAPHICSITEM_H
