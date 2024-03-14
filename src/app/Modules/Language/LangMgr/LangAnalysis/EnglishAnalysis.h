#ifndef ENGLISHANALYSIS_H
#define ENGLISHANALYSIS_H

#include "BaseFactory/MultiCharFactory.h"

namespace LangMgr {

    class EnglishAnalysis final : public MultiCharFactory {
        Q_OBJECT
    public:
        explicit EnglishAnalysis(QObject *parent = nullptr) : MultiCharFactory("English", parent) {
            m_language = English;
        }

        static bool contains(const QString &input);
    };

} // LangMgr

#endif // ENGLISHANALYSIS_H
