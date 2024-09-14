//
// Created by fluty on 2024/1/25.
//

#ifndef PITCHEDITORGRAPHICSITEM_H
#define PITCHEDITORGRAPHICSITEM_H

#include "Interface/IAtomicAction.h"
#include "Model/AppModel/DrawCurve.h"
#include "UI/Views/Common/OverlayGraphicsItem.h"

class CommonParamEditorView : public OverlayGraphicsItem, public IAtomicAction {
    Q_OBJECT

public:
    enum EditMode { Free, Anchor, Off };

    explicit CommonParamEditorView();

    void loadOriginal(const QList<DrawCurve *> &curves);
    void loadEdited(const QList<DrawCurve *> &curves);
    void clearParams();
    void setEraseMode(bool on);
    [[nodiscard]] const QList<DrawCurve *> &editedCurves() const;
    void cancelAction() override;
    void commitAction() override;

signals:
    void editCompleted(const QList<DrawCurve *> &curves);

protected:
    [[nodiscard]] bool fillCurve() const;
    void setFillCurve(bool on);
    [[nodiscard]] virtual double valueToSceneY(double value) const;
    [[nodiscard]] virtual double sceneYToValue(double y) const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void updateRectAndPos() override;
    void drawHandDrawCurves(QPainter *painter, const QList<DrawCurve *> &curves) const;
    static void drawLine(const QPoint &p1, const QPoint &p2, DrawCurve &curve);
    void drawCurve(QPainter *painter, const DrawCurve &curve) const;

    bool m_showDebugInfo = false;
    bool m_fillCurve = true;

    enum EditType { DrawOnInterval, DrawOnCurve, Erase, None };

    QPoint m_mouseDownPos; // x: tick, y: value
    QPoint m_prevPos;
    DrawCurve *m_editingCurve = nullptr;
    EditType m_editType = None;
    bool m_eraseMode = false;
    bool m_newCurveCreated = false;
    bool m_mouseMoved = false;
    QList<DrawCurve *> m_drawCurvesEdited;
    QList<DrawCurve *> m_drawCurvesOriginal;
    QList<DrawCurve *> m_drawCurvesEditedBak;

    [[nodiscard]] double startTick() const;
    [[nodiscard]] double endTick() const;
    [[nodiscard]] double sceneXToTick(double x) const;
    [[nodiscard]] double tickToSceneX(double tick) const;
    [[nodiscard]] double sceneXToItemX(double x) const;
    [[nodiscard]] double tickToItemX(double tick) const;
    [[nodiscard]] double sceneYToItemY(double y) const;
    [[nodiscard]] double valueToItemY(double value) const;
    DrawCurve *curveAt(double tick);
    QList<DrawCurve *> curvesIn(int startTick, int endTick);

    const int paddingTopBottom = 6;
};

#endif // PITCHEDITORGRAPHICSITEM_H
