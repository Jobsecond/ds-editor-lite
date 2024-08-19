//
// Created by fluty on 2024/1/24.
//

#ifndef PIANOROLLBACKGROUNDGRAPHICSITEM_H
#define PIANOROLLBACKGROUNDGRAPHICSITEM_H

#include "UI/Views/Common/TimeGridGraphicsItem.h"

class PianoRollBackground final : public TimeGridGraphicsItem {
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};



#endif // PIANOROLLBACKGROUNDGRAPHICSITEM_H
