#ifndef DS_EDITOR_LITE_LYRICDIALOG_H
#define DS_EDITOR_LITE_LYRICDIALOG_H

#include <QTabWidget>

#include <Model/Note.h>

#include "../../../Modules/Language/FillLyric/View/LyricTab.h"
#include "UI/Dialogs/Options/Pages/LanguagePage.h"

#include "UI/Dialogs/Base/Dialog.h"
#include "UI/Controls/Button.h"

class LyricDialog final : public Dialog {
    Q_OBJECT
public:
    explicit LyricDialog(QList<Note *> note, QWidget *parent = nullptr);
    ~LyricDialog() override;

    void exportLangNotes();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void noteToPhonic();

    void shrinkWindowRight(const int &newWidth);
    void expandWindowRight();

    void switchTab(const int &index);

    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;

    FillLyric::LyricTab *m_lyricWidget;
    LanguagePage *m_langPage;

    Button *m_btnOk;
    Button *m_btnCancel;

    QList<Note *> m_notes;
    QList<LangNote *> m_langNotes;
};

#endif // DS_EDITOR_LITE_LYRICDIALOG_H
