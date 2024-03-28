#ifndef LYRICCELL_H
#define LYRICCELL_H

#include <QObject>
#include <QApplication>

#include <QGraphicsView>
#include <QGraphicsObject>

#include <LangCommon.h>

namespace LyricWrap {
    class LyricCell final : public QGraphicsObject {
        Q_OBJECT
    public:
        explicit LyricCell(const qreal &x, const qreal &y, LangNote *note, QGraphicsView *view,
                           QGraphicsItem *parent = nullptr);
        ~LyricCell() override;

        [[nodiscard]] QString lyric() const;
        [[nodiscard]] QString syllable() const;

        void setFont(const QFont &font);
        void setLyricRect(const QRect &rect);
        void setSyllableRect(const QRect &rect);

        [[nodiscard]] qreal width() const;
        [[nodiscard]] qreal height() const;

        void setLyric(const QString &lyric) const;
        void setSyllable(const QString &syllable) const;

    Q_SIGNALS:
        void updateLyricSignal() const;
        void updateWidthSignal(const qreal &w) const;

    protected:
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

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

        enum PenType { Multitone = 1, Revised, G2pError };
        QPen m_lyricPen[4] = {QColor(Qt::black), QColor(Qt::green), QColor(Qt::yellow),
                              QColor(Qt::red)};
        QPen m_syllablePen[4] = {QColor(Qt::white), QColor(Qt::green), QColor(Qt::yellow),
                                 QColor(Qt::red)};

        [[nodiscard]] qreal syllableWidth() const;
        [[nodiscard]] qreal lyricWidth() const;

        [[nodiscard]] QPointF syllablePos() const;
        [[nodiscard]] QPointF lyricPos() const;
        [[nodiscard]] QRectF lyricRect() const;
        [[nodiscard]] QPointF rectPos() const;

        void changePhonicMenu(QMenu *menu);
        void changeSyllableMenu(QMenu *menu) const;

        QRect m_lRect;
        QRect m_sRect;

        LangNote *m_note;
        QGraphicsView *m_view;

        qreal m_lsPadding = 5;
        qreal m_rectPadding = 3;
        qreal m_padding = 5;
        qreal m_reckBorder = 2.5;

        QString mText;
        QFont m_font = QApplication::font();

        bool isLyricEditing = false;

        qreal m_width{};
        qreal m_height{};
    };
}

#endif // LYRICCELL_H
