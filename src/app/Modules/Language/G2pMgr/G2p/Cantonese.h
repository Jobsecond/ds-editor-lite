#ifndef CANTONESE_H
#define CANTONESE_H

#include <QObject>

#include <cantonese.h>

#include "../IG2pFactory.h"

namespace G2pMgr {

    class Cantonese final : public IG2pFactory {
        Q_OBJECT
    public:
        explicit Cantonese(QObject *parent = nullptr);
        ~Cantonese() override;

        QList<Phonic> convert(QStringList &input) const override;

    private:
        IKg2p::Cantonese *m_cantonese;
    };

} // G2pMgr

#endif // CANTONESE_H