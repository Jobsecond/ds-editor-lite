#ifndef MANDARIN_H
#define MANDARIN_H

#include <QObject>

#include <mandarin.h>

#include "../IG2pFactory.h"

namespace G2pMgr {

    class Mandarin final : public IG2pFactory {
        Q_OBJECT
    public:
        explicit Mandarin(QObject *parent = nullptr);
        ~Mandarin() override;

        QList<Phonic> convert(QStringList &input) const override;

        QJsonObject config() override;
        QWidget *configWidget(QJsonObject *config) override;

    private:
        IKg2p::Mandarin *m_mandarin;

        bool tone = false;
        bool convertNum = false;
    };
} // G2pMgr

#endif // MANDARIN_H
