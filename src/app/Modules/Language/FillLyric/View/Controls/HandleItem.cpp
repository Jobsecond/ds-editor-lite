#include "HandleItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

namespace FillLyric {
    HandleItem::HandleItem(QGraphicsItem *parent) : QGraphicsRectItem(parent) {
        setFlag(ItemIsSelectable);
        this->setAcceptHoverEvents(true);
    }

    HandleItem::~HandleItem() = default;

    void HandleItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            for (const auto item : scene()->selectedItems()) {
                item->setSelected(false);
            }
        }
        Q_EMIT selectAll();
        this->setSelected(true);
    }

    void HandleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            for (const auto item : scene()->selectedItems()) {
                item->setSelected(false);
            }
        }
        Q_EMIT selectAll();
        this->setSelected(true);
    }

    QRectF HandleItem::boundingRect() const {
        return {0, 0, width(), height()};
    }

    QPainterPath HandleItem::shape() const {
        QPainterPath path;
        path.addRect({m_margin, m_margin, width() - m_margin * 2, height() - m_margin * 2});
        return path;
    }

    void HandleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                           QWidget *widget) {
        int flag = 0;
        if (option->state & QStyle::State_MouseOver)
            flag = Hovered;
        if (option->state & QStyle::State_Selected)
            flag = Selected;

        const auto boxRect =
            QRectF(m_margin, m_margin, width() - m_margin * 2, height() - m_margin * 2);

        painter->setPen(m_borderPen[flag]);
        painter->setBrush(m_backgroundBrush[flag]);
        painter->drawRoundedRect(boxRect, m_margin * 0.5, m_margin * 0.5);
    }

    void HandleItem::setWidth(const qreal &w) {
        mW = w;
        this->setRect({x(), y(), mW, mH});
    }

    qreal HandleItem::width() const {
        return mW;
    }

    void HandleItem::setHeight(const qreal &h) {
        mH = h;
        this->setRect({x(), y(), mW, mH});
    }

    qreal HandleItem::height() const {
        return mH;
    }

    void HandleItem::setMargin(const qreal &margin) {
        m_margin = margin;
    }

    qreal HandleItem::margin() const {
        return m_margin;
    }


} // FillLyric