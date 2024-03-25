#ifndef DS_EDITOR_LITE_LYRICWIDGET_H
#define DS_EDITOR_LITE_LYRICWIDGET_H

#include <QWidget>

#include "Widgets/LyricBaseWidget.h"
#include "Widgets/LyricExtWidget.h"

#include "../Model/PhonicCommon.h"

class LineEdit;

namespace FillLyric {
    class LyricTab final : public QWidget {
        Q_OBJECT
        friend class LyricDialog;

    public:
        explicit LyricTab(QList<Phonic *> phonics, QWidget *parent = nullptr);
        ~LyricTab() override;

        LyricBaseWidget *m_lyricBaseWidget;
        LyricExtWidget *m_lyricExtWidget;

        void setPhonics();

        [[nodiscard]] QList<Phonic> exportPhonics() const;
        [[nodiscard]] QList<Phonic> modelExport() const;

        [[nodiscard]] bool exportSkipSlur() const;
        [[nodiscard]] bool exportLanguage() const;

    Q_SIGNALS:
        void shrinkWindowRight(int newWidth);
        void expandWindowRight();

    public Q_SLOTS:
        void _on_btnInsertText_clicked() const;
        void _on_btnToTable_clicked() const;
        void _on_btnToText_clicked() const;

    private:
        void modifyOption() const;

        QList<Phonic *> m_phonics;

        // Variables
        int notesCount = 0;

        // Layout
        QVBoxLayout *m_mainLayout;
        QHBoxLayout *m_lyricLayout;
    };

} // FillLyric

#endif // DS_EDITOR_LITE_LYRICWIDGET_H
