//
// Created by fluty on 2024/1/23.
//

#ifndef COMMONGRAPHICSVIEW_H
#define COMMONGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QTimer>

class CommonGraphicsView : public QGraphicsView {
    Q_OBJECT
    Q_PROPERTY(double scaleX READ scaleX WRITE setScaleX)
    Q_PROPERTY(double scaleY READ scaleY WRITE setScaleY)
    Q_PROPERTY(double horizontalScrollBarValue READ hBarValue WRITE setHBarValue)
    Q_PROPERTY(double verticalScrollBarValue READ vBarValue WRITE setVBarValue)

public:
    explicit CommonGraphicsView(QWidget *parent = nullptr);
    ~CommonGraphicsView() override = default;

    double scaleX() const;
    void setScaleX(double sx);
    double scaleY() const;
    void setScaleY(double sy);
    void setScale(const double sx, const double sy) {
        setScaleX(sx);
        setScaleY(sy);
    }
    double scaleXMax() const;
    void setScaleXMax(double max);
    double scaleYMin() const;
    void setScaleYMin(double min);
    int hBarValue() const;
    void setHBarValue(int value);
    int vBarValue() const;
    void setVBarValue(int value);
    QRectF visibleRect() const;
    void setEnsureSceneFillView(bool on);

signals:
    void scaleChanged(double sx, double sy);
    void visibleRectChanged(const QRectF &rect);
    void sizeChanged(QSize size);

public slots:
    void notifyVisibleRectChanged();
    void onWheelHorScale(QWheelEvent *event);

protected:
    bool event(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    // bool eventFilter(QObject *object, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool isMouseEventFromWheel(QWheelEvent *event);

    double m_hZoomingStep = 0.4;
    double m_vZoomingStep = 0.3;
    double m_scaleX = 1;
    double m_scaleY = 1;
    // double m_scaleXMin = 0.1;
    double m_scaleXMax = 3; // 3x
    double m_scaleYMin = 0.5;
    double m_scaleYMax = 8;
    bool m_ensureSceneFillView = true;

    QPropertyAnimation m_scaleXAnimation;
    QPropertyAnimation m_scaleYAnimation;
    QPropertyAnimation m_hBarAnimation;
    QPropertyAnimation m_vBarAnimation;

    QTimer m_timer;
    bool m_touchPadLock = false;
};

#endif // COMMONGRAPHICSVIEW_H
