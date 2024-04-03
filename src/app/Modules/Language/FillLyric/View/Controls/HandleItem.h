#ifndef HANDLEITEM_H
#define HANDLEITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>

namespace FillLyric {
    class HandleItem final : public QObject, public QGraphicsItem {
        Q_OBJECT
    public:
        explicit HandleItem(QGraphicsItem *parent = nullptr);
        ~HandleItem() override;

        [[nodiscard]] qreal width() const;
        void setWidth(const qreal &w);

        [[nodiscard]] qreal height() const;
        void setHeight(const qreal &h);

        [[nodiscard]] qreal deltaX() const;

        void setMargin(const qreal &margin);
        [[nodiscard]] qreal margin() const;

    Q_SIGNALS:
        void selectAll() const;

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

        [[nodiscard]] QRectF boundingRect() const override;
        [[nodiscard]] QPainterPath shape() const override;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;

    private:
        enum State {
            Normal = 0,
            Hovered = 1,
            Selected = 2,
        };

        QBrush m_backgroundBrush[3] = {QColor(255, 255, 255, 25), QColor(255, 255, 255, 40),
                                       QColor(255, 255, 255, 40)};
        QPen m_borderPen[3] = {QPen(QColor(255, 255, 255, 35), 2), QPen(QColor(112, 156, 255), 2),
                               QPen(QColor(112, 156, 255), 2)};

        qreal mW = 16;
        qreal mH = 0;

        qreal m_margin = 4;
    };

} // FillLyric

#endif // HANDLEITEM_H
