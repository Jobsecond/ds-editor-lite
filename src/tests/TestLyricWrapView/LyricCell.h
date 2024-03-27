#ifndef LYRICCELL_H
#define LYRICCELL_H

#include <QObject>
#include <QPainter>

#include <LangCommon.h>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

#include <QApplication>

#include "LyricLineEdit.h"

namespace LyricWrap {
    class LyricCell final : public QObject, public QGraphicsItem {
        Q_OBJECT
    public:
        explicit LyricCell(const qreal &x, const qreal &y, LangNote *note,
                           QGraphicsItem *parent = nullptr);
        ~LyricCell() override;

        [[nodiscard]] qreal width() const;
        [[nodiscard]] qreal height() const;

        void setPos(const qreal &x, const qreal &y);

        void setLyric(const QString &lyric);

    Q_SIGNALS:
        void updateWidthSignal(const qreal &w) const;

    protected:
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;

        [[nodiscard]] QRectF boundingRect() const override;

    private:
        enum State {
            Normal = 0,
            Hovered = 1,
            Selected = 2,
        };

        QBrush m_backgroundBrush[3] = {QColor(155, 186, 255), QColor(169, 196, 255),
                                       QColor(169, 196, 255)};
        QPen m_borderPen[3] = {QPen(QColor(112, 156, 255), 2), QPen(QColor(112, 156, 255), 2),
                               QPen(QColor(Qt::white), 2)};
        QPen m_lyricPen[3] = {QColor(Qt::black), QColor(Qt::black), QColor(Qt::black)};
        QPen m_syllablePen[3] = {QColor(Qt::white), QColor(Qt::white), QColor(Qt::white)};

        void updateWidth();
        void updateHeight();

        [[nodiscard]] int syllableWidth() const;
        [[nodiscard]] int syllableHeight() const;
        [[nodiscard]] int lyricWidth() const;
        [[nodiscard]] int lyricHeight() const;

        [[nodiscard]] QPointF syllablePos() const;
        [[nodiscard]] QPointF lyricPos() const;
        [[nodiscard]] QPointF rectPos() const;

        void updateLyric();

        qreal mX;
        qreal mY;

        qreal m_lsPadding = 3;
        qreal m_rectPadding = 5;
        qreal m_padding = 5;

        LangNote *m_note;

        QGraphicsTextItem *m_lyricItem;
        QGraphicsRectItem *m_rectItem;
        QGraphicsItemGroup *m_group;

        QString mText;
        QFont m_font = QApplication::font();

        bool isLyricEditing = false;
        QPointer<LyricLineEdit> m_lyricEdit = new LyricLineEdit();
        QPointer<QGraphicsProxyWidget> m_lyricWidget = new QGraphicsProxyWidget();

        qreal m_width{};
        qreal m_height{};
    };
}

#endif // LYRICCELL_H
