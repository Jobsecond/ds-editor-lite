//
// Created by fluty on 24-3-18.
//

#ifndef GENERALPAGE_H
#define GENERALPAGE_H

#include "IOptionPage.h"

class Button;
class LineEdit;
class LanguageComboBox;
class FileSelector;

class GeneralPage : public IOptionPage {
    Q_OBJECT

public:
    explicit GeneralPage(QWidget *parent = nullptr);

protected:
    void modifyOption() override;

private:
    Button *m_btnOpenConfigFolder;
    LanguageComboBox *m_cbDefaultSingingLanguage;
    LineEdit *m_leDefaultLyric;
    FileSelector *m_fsDefaultSinger;

    FileSelector *m_fsSomePath;
    FileSelector *m_fsRmvpePath;
};

#endif // GENERALPAGE_H
