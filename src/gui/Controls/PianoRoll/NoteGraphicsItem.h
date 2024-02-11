//
// Created by fluty on 2024/1/23.
//

#ifndef NOTEGRAPHICSITEM_H
#define NOTEGRAPHICSITEM_H

#include "PianoRollGlobal.h"
#include "Controls/Base/CommonGraphicsRectItem.h"
#include "Utils/IOverlapable.h"
#include "Utils/UniqueObject.h"

class NoteGraphicsItem final : public CommonGraphicsRectItem, public UniqueObject, public IOverlapable {
    Q_OBJECT

public:
    enum MouseMoveBehavior { Move, ResizeRight, ResizeLeft, None };

    explicit NoteGraphicsItem(int itemId, QGraphicsItem *parent = nullptr);
    explicit NoteGraphicsItem(int itemId, int start, int length, int keyIndex, const QString &lyric,
                              const QString &pronunciation, QGraphicsItem *parent = nullptr);

    QWidget *context() const;
    void setContext(QWidget *context);

    int start() const;
    void setStart(int start);
    int length() const;
    void setLength(int length);
    int keyIndex() const;
    void setKeyIndex(int keyIndex);
    QString lyric() const;
    void setLyric(const QString &lyric);
    QString pronunciation() const;
    void setPronunciation(const QString &pronunciation);

signals:
    void editLyricTriggered(int id);
    void removeTriggered(int id);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void updateRectAndPos() override;
    void initUi();
    // void addMenuActions(QMenu *menu);

    QWidget *m_context;
    QMenu *m_menu;
    bool m_propertyEdited = false;
    int m_start = 0;
    int m_length = 480;
    int m_keyIndex = 60;
    QString m_lyric;
    QString m_pronunciation;

    MouseMoveBehavior m_mouseMoveBehavior = Move;
    QPointF m_mouseDownPos;
    int m_mouseDownStart;
    int m_mouseDownLength;
    int m_mouseDownKeyIndex;

    int m_quantize = 240;
    bool m_tempQuantizeOff = false;
    int m_resizeTolerance = 8; // px
    int m_pronunciationTextHeight = 20;
};

#endif // NOTEGRAPHICSITEM_H
