//
// Created by fluty on 2024/1/27.
//

#ifndef DSPARAMS_H
#define DSPARAMS_H

#include "Curve.h"
#include "Utils/OverlappableSerialList.h"

class Param {
public:
    enum ParamType { Original, Edited, Envelope, Unknown };

    [[nodiscard]] const QList<Curve *> &curves(ParamType type) const;
    void setCurves(ParamType type, const QList<Curve *> &curves);

private:
    QList<Curve *> m_original;
    QList<Curve *> m_edited;
    QList<Curve *> m_envelope;
    QList<Curve *> m_unknown;
};

class ParamBundle {
public:
    enum ParamName { Pitch, Energy, Tension, Breathiness, Unknown };

    Param pitch;
    Param energy;
    Param tension;
    Param breathiness;

    Param *getParamByName(ParamName name);

private:
    Param unknown;
};



#endif // DSPARAMS_H
