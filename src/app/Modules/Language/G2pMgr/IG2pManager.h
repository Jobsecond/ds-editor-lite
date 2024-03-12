#ifndef IG2PMANAGER_H
#define IG2PMANAGER_H

#include <QObject>

#include "IG2pFactory.h"

namespace G2pMgr {

    class IG2pManagerPrivate;

    class IG2pManager final : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(IG2pManager)
    public:
        explicit IG2pManager(QObject *parent = nullptr);
        ~IG2pManager() override;

        static IG2pManager *instance();

    public:
        IG2pFactory *g2p(const QString &id) const;
        bool addG2p(IG2pFactory *factory);
        bool removeG2p(const IG2pFactory *factory);
        bool removeG2p(const QString &id);
        void clearG2ps();
        [[nodiscard]] QList<IG2pFactory *> g2ps() const;

    private:
        explicit IG2pManager(IG2pManagerPrivate &d, QObject *parent = nullptr);

        QScopedPointer<IG2pManagerPrivate> d_ptr;
    };

}

#endif // IG2PMANAGER_H