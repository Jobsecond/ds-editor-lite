//
// Created by fluty on 24-2-23.
//

#ifndef PARAMSACTIONS_H
#define PARAMSACTIONS_H

#include "Modules/History/ActionSequence.h"
#include "Model/Params.h"

class SingingClip;

class ParamsActions : public ActionSequence {
public:
    void replaceParam(ParamBundle::ParamName paramName, Param::ParamType paramType,
                      const OverlapableSerialList<Curve> &curves, SingingClip* clip);
    void replacePitchOriginal(const OverlapableSerialList<Curve> &curves, SingingClip* clip);
    void replacePitchEdited(const OverlapableSerialList<Curve> &curves, SingingClip* clip);
};

#endif // PARAMSACTIONS_H
