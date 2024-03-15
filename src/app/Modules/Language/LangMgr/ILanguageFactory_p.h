#ifndef ILANGUAGEFACTORY_P_H
#define ILANGUAGEFACTORY_P_H

#include "ILanguageFactory.h"

namespace LangMgr {

    class ILanguageFactoryPrivate final {
        Q_DECLARE_PUBLIC(ILanguageFactory)
    public:
        ILanguageFactoryPrivate();
        virtual ~ILanguageFactoryPrivate();

        void init();

        ILanguageFactory *q_ptr;

        QString id;

        bool discard = false;

        QString displayName;
        QString description;

        QString category;
        QString displayCategory;
    };

}

#endif // ILANGUAGEFACTORY_P_H
