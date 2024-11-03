#ifndef UNKNOWSET_H
#define UNKNOWSET_H

#include "../IG2pSetFactory.h"

namespace LangSetting {

    class UnknownG2pSet final : public IG2pSetFactory {
        Q_OBJECT

    public:
        explicit UnknownG2pSet(QObject *parent = nullptr) : IG2pSetFactory("unknown", parent) {
        }

        QWidget *g2pConfigWidget(const QJsonObject &config) override;
    };

} // LangSetting

#endif // UNKNOWSET_H