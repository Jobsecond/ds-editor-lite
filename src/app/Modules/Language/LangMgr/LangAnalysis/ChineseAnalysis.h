#ifndef CHINESEANALYSIS_H
#define CHINESEANALYSIS_H

#include "BaseFactory/SingleCharFactory.h"

namespace LangMgr {

    class ChineseAnalysis final : public SingleCharFactory {
        Q_OBJECT
    public:
        explicit ChineseAnalysis(const QString &id = "Mandarin", QObject *parent = nullptr)
            : SingleCharFactory(id, parent) {
            m_language = Mandarin;
        }

        [[nodiscard]] bool contains(const QChar &c) const override;
    };

} // LangMgr

#endif // CHINESEANALYSIS_H
