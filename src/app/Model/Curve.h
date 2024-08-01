//
// Created by fluty on 2024/1/27.
//

#ifndef DSCURVE_H
#define DSCURVE_H

#include <QList>

#include "Utils/Overlappable.h"
#include "Utils/ISelectable.h"
#include "Utils/UniqueObject.h"

class QPoint;

class Curve : public Overlappable, public UniqueObject {
public:
    enum CurveType { Generic, Draw, Anchor };

    Curve() = default;
    explicit Curve(int id) : UniqueObject(id) {
    }
    Curve(const Curve &other) : UniqueObject(other.id()), m_start(other.m_start) {
    }
    virtual ~Curve() = default;

    virtual CurveType type() {
        return Generic;
    }
    [[nodiscard]] int start() const;
    void setStart(int offset);

    int compareTo(Curve *obj) const;
    [[nodiscard]] virtual int endTick() const {
        return m_start;
    }
    bool isOverlappedWith(Curve *obj) const;
    [[nodiscard]] std::tuple<qsizetype, qsizetype> interval() const override;

private:
    int m_start = 0;
};

class DrawCurve final : public Curve {
public:
    DrawCurve() = default;
    explicit DrawCurve(int id) : Curve(id) {
    }
    DrawCurve(const DrawCurve &other);

    CurveType type() override {
        return Draw;
    }
    int step = 5; // TODO: remove
    [[nodiscard]] const QList<int> &values() const;
    void setValues(const QList<int> &values);
    void insertValue(int index, int value);
    void insertValues(int index, const QList<int> &values);
    void removeValueRange(int i, int n);
    void clearValues();
    void appendValue(int value);
    void replaceValue(int index, int value);
    void mergeWith(const DrawCurve &other);
    void overlayMergeWith(const DrawCurve &other);

    [[nodiscard]] int endTick() const override;

private:
    // int m_step = 5;
    QList<int> m_values;
};

class ProbeLine final : public Curve {
public:
    ProbeLine() : Curve(-1) {
    }
    void setEndTick(int tick);
    [[nodiscard]] int endTick() const override;

private:
    int m_endTick = 0;
};

class AnchorNode : public Overlappable, public UniqueObject, public ISelectable {
public:
    enum InterpMode { Linear, Hermite, Cubic, None };

    AnchorNode(const int pos, const int value) : m_pos(pos), m_value(value) {
    }

    [[nodiscard]] int pos() const;
    void setPos(int pos);
    [[nodiscard]] int value() const;
    void setValue(int value);
    [[nodiscard]] InterpMode interpMode() const;
    void setInterpMode(InterpMode mode);

    int compareTo(AnchorNode *obj) const;
    bool isOverlappedWith(AnchorNode *obj) const;
    [[nodiscard]] std::tuple<qsizetype, qsizetype> interval() const override;

private:
    int m_pos;
    int m_value;
    InterpMode m_interpMode = Cubic;
};

class AnchorCurve final : public Curve {
public:
    CurveType type() override {
        return Anchor;
    }
    // TODO: use OverlapableSerialList
    [[nodiscard]] const QList<AnchorNode *> &nodes() const;
    void insertNode(AnchorNode *node);
    void removeNode(AnchorNode *node);
    [[nodiscard]] int endTick() const override;

private:
    QList<AnchorNode *> m_nodes;
};

#endif // DSCURVE_H
