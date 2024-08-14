//
// Created by fluty on 2024/1/27.
//

#include "Curve.h"

#include <QDebug>

int Curve::compareTo(const Curve *obj) const {
    int otherStart = obj->start;
    if (start < otherStart)
        return -1;
    if (start > otherStart)
        return 1;
    return 0;
}
bool Curve::isOverlappedWith(Curve *obj) const {
    int otherStart = obj->start;
    auto otherEnd = obj->endTick();
    if (otherEnd <= start || endTick() <= otherStart)
        return false;
    return true;
}
std::tuple<qsizetype, qsizetype> Curve::interval() const {
    return std::make_tuple(start, endTick());
}
void ProbeLine::setEndTick(int tick) {
    m_endTick = tick;
}
int ProbeLine::endTick() const {
    return m_endTick;
}
int AnchorNode::pos() const {
    return m_pos;
}
void AnchorNode::setPos(int pos) {
    m_pos = pos;
}
int AnchorNode::value() const {
    return m_value;
}
void AnchorNode::setValue(int value) {
    m_value = value;
}
AnchorNode::InterpMode AnchorNode::interpMode() const {
    return m_interpMode;
}
void AnchorNode::setInterpMode(InterpMode mode) {
    m_interpMode = mode;
}
int AnchorNode::compareTo(const AnchorNode *obj) const {
    auto otherPos = obj->pos();
    if (pos() < otherPos)
        return -1;
    if (pos() > otherPos)
        return 1;
    return 0;
}
bool AnchorNode::isOverlappedWith(const AnchorNode *obj) const {
    return pos() == obj->pos();
}
std::tuple<qsizetype, qsizetype> AnchorNode::interval() const {
    return std::make_tuple(pos(), pos());
}
const QList<AnchorNode *> &AnchorCurve::nodes() const {
    return m_nodes;
}
void AnchorCurve::insertNode(AnchorNode *node) {
    m_nodes.append(node);
}
void AnchorCurve::removeNode(AnchorNode *node) {
    m_nodes.removeOne(node);
}
int AnchorCurve::endTick() const {
    return m_nodes.at(m_nodes.count() - 1)->pos(); // TODO: fix
}