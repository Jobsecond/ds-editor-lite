#include "LyricDialog.h"

#include "LangMgr/ILanguageManager.h"
#include "LangMgr/LangAnalysis/CantoneseAnalysis.h"

#include <QApplication>

#include "Model/Note.h"
#include "Model/AppOptions/AppOptions.h"
#include "UI/Controls/AccentButton.h"
#include "UI/Dialogs/Options/Pages/LanguagePage.h"

LyricDialog::LyricDialog(QList<Note *> note, QWidget *parent)
    : Dialog(parent), m_notes(std::move(note)) {
    setModal(true);
    setMinimumSize(720, 450);
    setWindowTitle(tr("Fill Lyric"));
    setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

    const auto size = QApplication::primaryScreen()->availableSize();
    resize(static_cast<int>(size.width() * 0.6), static_cast<int>(size.height() * 0.6));

    noteToPhonic();

    m_mainLayout = new QVBoxLayout();
    m_tabWidget = new QTabWidget();

    m_lyricWidget = new FillLyric::LyricTab(m_langNotes);
    m_lyricWidget->setLangNotes();

    if (!AppOptions::instance()->fillLyric()->extVisible) {
        shrinkWindowRight(300);
    }

    m_langPage = new LanguagePage(this);

    m_btnOk = new AccentButton(tr("&Import"), this);
    // m_btnOk->setPrimary(true);
    setPositiveButton(m_btnOk);
    m_btnCancel = new Button(tr("&Cancel"), this);
    setNegativeButton(m_btnCancel);

    m_tabWidget->addTab(m_lyricWidget, tr("Lyric"));
    m_tabWidget->addTab(m_langPage, tr("Advanced"));
    m_tabWidget->addTab(new QWidget, tr("Help"));

    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->setContentsMargins({});
    body()->setLayout(m_mainLayout);

    connect(m_btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_lyricWidget, &FillLyric::LyricTab::shrinkWindowRight, this,
            &LyricDialog::shrinkWindowRight);
    connect(m_lyricWidget, &FillLyric::LyricTab::expandWindowRight, this,
            &LyricDialog::expandWindowRight);

    connect(m_tabWidget, &QTabWidget::currentChanged, this, &LyricDialog::switchTab);
}

LyricDialog::~LyricDialog() = default;

void LyricDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;
    QDialog::keyPressEvent(event);
}

void LyricDialog::noteToPhonic() {
    const auto langMgr = LangMgr::ILanguageManager::instance();
    for (const auto note : m_notes) {
        auto langNote = LangNote(note->lyric());
        langNote.language =
            note->language() != "Unknown" ? note->language() : langMgr->analysis(note->lyric());
        langNote.category = langMgr->language(langNote.language)->category();
        langNote.syllable = note->pronunciation().original;
        langNote.syllableRevised = note->pronunciation().edited;
        langNote.candidates = note->pronCandidates();

        if (note->isSlur()) {
            langNote.language = "Slur";
            langNote.category = "Slur";
        }

        m_langNotes.append(langNote);
    }
}

void LyricDialog::shrinkWindowRight(const int &newWidth) {
    setMinimumSize(300, 450);
    resize(newWidth, height());
}

void LyricDialog::expandWindowRight() {
    setMinimumSize(720, 450);
    const auto size = QApplication::primaryScreen()->availableSize();
    resize(static_cast<int>(size.width() * 0.6), height());
}

QList<LangNote> LyricDialog::exportLangNotes() {
    const auto noteLists = m_lyricWidget->exportLangNotes();

    const bool skipSlurRes = m_lyricWidget->exportSkipSlur();

    const bool exportLangRes = m_lyricWidget->exportLanguage();

    QList<LangNote> result;
    for (const auto &langNotes : noteLists) {
        for (auto& langNote : langNotes) {
            result.append(langNote);
        }
    }
    return result;
}

void LyricDialog::switchTab(const int &index) {
    if (index == 0) {
        if (!m_lyricWidget->m_lyricExtWidget->isVisible()) {
            this->shrinkWindowRight(300);
        }
    } else {
        if (!m_lyricWidget->m_lyricExtWidget->isVisible()) {
            this->expandWindowRight();
        }
    }
}