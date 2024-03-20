#ifndef ENGLISHANALYSIS_H
#define ENGLISHANALYSIS_H

#include "BaseFactory/MultiCharFactory.h"

namespace LangMgr {

    class EnglishAnalysis final : public MultiCharFactory {
        Q_OBJECT
    public:
        explicit EnglishAnalysis(QObject *parent = nullptr) : MultiCharFactory("English", parent) {
        }

        [[nodiscard]] bool contains(const QChar &c) const override;
        [[nodiscard]] bool contains(const QString &input) const override;
    };

} // LangMgr

#endif // ENGLISHANALYSIS_H
