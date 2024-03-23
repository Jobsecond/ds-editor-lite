#include "LangMgrWidget.h"

#include "Modules/Language/LangMgr/ILanguageManager.h"

namespace LangMgr {
    LangMgrWidget::LangMgrWidget(QWidget *parent) : QWidget(parent) {
        m_mainLayout = new QVBoxLayout();
        m_tableLayout = new QVBoxLayout();

        m_langListWidget = new LangListWidget();

        m_buttonLayout = new QHBoxLayout();
        m_applyButton = new Button(tr("Apply"));
        m_buttonLayout->addStretch(1);
        m_buttonLayout->addWidget(m_applyButton);

        m_centerLayout = new QHBoxLayout();

        m_langInfoWidget = new LangInfoWidget();
        m_g2pInfoWidget = new G2pInfoWidget();

        m_centerLayout->addWidget(m_langListWidget, 1);
        m_centerLayout->addWidget(m_langInfoWidget, 2);
        m_centerLayout->addWidget(m_g2pInfoWidget, 2);

        m_tableLayout->addLayout(m_centerLayout);
        m_tableLayout->addLayout(m_buttonLayout);

        m_labelLayout = new QVBoxLayout();
        m_labelLayout->setContentsMargins(0, 0, 0, 0);
        const auto text =
            tr("The original text is analyzed by various language analyzers to identify the "
               "corresponding language in sequence;\n"
               "If a certain analyzer is disabled in the first column, it will not participate in "
               "word segmentation;\n"
               "If \"Discard Result\" is checked, the results of word segmentation and analysis "
               "will not enter the notes.");
        m_label = new QLabel(this);
        m_label->setText(text);
        m_label->setWordWrap(true);
        m_labelLayout->addWidget(m_label);

        m_mainLayout->addLayout(m_labelLayout);
        m_mainLayout->addLayout(m_tableLayout);
        setLayout(m_mainLayout);

        m_langListWidget->setCurrentIndex(m_langListWidget->model()->index(0, 0));

        const auto langId = LangMgr::ILanguageManager::instance()->languageOrder().first();
        m_langInfoWidget->setInfo(langId);

        const auto g2pId = LangMgr::ILanguageManager::instance()->language(langId)->selectedG2p();
        m_g2pInfoWidget->setInfo(langId, g2pId);

        connect(m_langListWidget, &QListWidget::currentRowChanged, m_langInfoWidget, [this] {
            m_langInfoWidget->setInfo(
                m_langListWidget->currentItem()->data(Qt::UserRole).toString());
        });
        connect(m_langInfoWidget, &LangInfoWidget::g2pSelected, m_g2pInfoWidget,
                &G2pInfoWidget::setInfo);
    }

    LangMgrWidget::~LangMgrWidget() = default;
} // LangMgr