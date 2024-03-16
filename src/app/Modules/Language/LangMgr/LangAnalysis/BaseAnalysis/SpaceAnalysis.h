#ifndef SPACEANALYSIS_H
#define SPACEANALYSIS_H

#include "../BaseFactory/SingleCharFactory.h"

namespace LangMgr {

    class SpaceAnalysis final : public SingleCharFactory {
    public:
        explicit SpaceAnalysis(const QString &id = "Space", QObject *parent = nullptr)
            : SingleCharFactory(id, parent) {
            setDiscardResult(true);
        }

        [[nodiscard]] bool contains(const QChar &c) const override;
    };

} // LangMgr

#endif // SPACEANALYSIS_H
