#ifndef ILANGUAGEMANAGER_P_H
#define ILANGUAGEMANAGER_P_H

#include <QObject>
#include <QMap>

#include "ILanguageManager.h"

namespace LangMgr {

    class ILanguageManagerPrivate final : public QObject {
        Q_OBJECT
        Q_DECLARE_PUBLIC(ILanguageManager)
    public:
        ILanguageManagerPrivate();
        ~ILanguageManagerPrivate() override;

        void init();

        bool initialized = false;

        ILanguageManager *q_ptr;

        QStringList category = {"Mandarin", "Cantonese", "Kana", "English", "Unknown"};

        QStringList order = {"Mandarin", "Pinyin", "Cantonese",   "Kana",   "Romaji",    "English",
                             "Space",    "Slur",   "Punctuation", "Number", "Linebreak", "Unknown"};

        QMap<QString, ILanguageFactory *> languages;
    };

} // LangMgr

#endif // ILANGUAGEMANAGER_P_H
