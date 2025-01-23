//
// Created by FlutyDeer on 25-1-22.
//

#include "TestAnchorEditor.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>

TestAnchorEditor::TestAnchorEditor(QWidget *parent) {
    resize(1280, 400);

    curve.insert({100, 100});
    curve.insert({200, 50});
}

void TestAnchorEditor::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setColor({112, 156, 255});
    pen.setWidthF(1.5);

    // Draw curve
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    auto knots = curve.getKnots();
    auto startTick = knots.first().getPosition();
    auto endTick = knots.last().getPosition();

    QPainterPath curvePath;
    bool firstPoint = true;
    for (int i = startTick; i <= endTick; i++) {
        auto value = curve.getValue(i);
        // qDebug() << value;
        if (firstPoint) {
            curvePath.moveTo(i, value);
            firstPoint = false;
        }
        else
            curvePath.lineTo(i, value);
    }
    painter.drawPath(curvePath);
}

void TestAnchorEditor::mousePressEvent(QMouseEvent *event) {

    // QWidget::mousePressEvent(event);
}