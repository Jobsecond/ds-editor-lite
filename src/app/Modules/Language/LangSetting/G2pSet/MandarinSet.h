#ifndef MANDARINSET_H
#define MANDARINSET_H

#include "../IG2pSetFactory.h"

#include <LangMgr/IG2pManager.h>

namespace LangSetting {

    class MandarinSet final : public IG2pSetFactory {
        Q_OBJECT

    public:
        explicit MandarinSet(QObject *parent = nullptr) : IG2pSetFactory("cmn", parent) {
            d = dynamic_cast<LangMgr::IG2pFactory *>(LangMgr::IG2pManager::instance()->g2p(id()));
        }

        QWidget *configWidget(QJsonObject *config) override;

    private:
        LangMgr::IG2pFactory *d;
    };

} // LangSetting

#endif // MANDARINSET_H