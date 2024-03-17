#ifndef LANGTABLEWIDGETS_H
#define LANGTABLEWIDGETS_H

#include <QListWidget>

#include "../../LangCommon.h"

namespace LangMgr {

    class LangListWidget final : public QListWidget {
        Q_OBJECT
    public:
        explicit LangListWidget(QWidget *parent = nullptr);
        ~LangListWidget() override;

        [[nodiscard]] LangConfig currentConfig() const;

    protected:
        void dropEvent(QDropEvent *event) override;

        void setItem(const int &row, const LangConfig &langConfig);

        [[nodiscard]] LangConfig exportConfig(const int &row) const;
    };

} // LangMgr

#endif // LANGTABLEWIDGETS_H
