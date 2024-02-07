//
// Created by fluty on 2024/2/7.
//

#ifndef IPROJECTCONVERTER_H
#define IPROJECTCONVERTER_H

#include "Model/AppModel.h"

class IProjectConverter {
public:
    virtual bool load(const QString &path, AppModel *model, QString &errMsg) = 0;
    virtual bool save(const QString &path, AppModel *model, QString &errMsg) = 0;
    virtual ~IProjectConverter() = default;
};



#endif //IPROJECTCONVERTER_H
