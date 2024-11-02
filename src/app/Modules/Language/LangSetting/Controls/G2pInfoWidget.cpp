#include "G2pInfoWidget.h"

#include "Modules/Language/LangSetting/ILangSetManager.h"

#include <language-manager/IG2pManager.h>

namespace LangSetting {
    G2pInfoWidget::G2pInfoWidget(QWidget *parent) : QWidget(parent) {
        this->setContentsMargins(0, 0, 0, 0);

        this->m_mainLayout = new QVBoxLayout();
        this->m_mainLayout->setContentsMargins(0, 0, 0, 0);
        this->m_topLayout = new QVBoxLayout();
        this->m_authorLayout = new QHBoxLayout();

        this->m_label = new QLabel(tr("G2P Config"));

        this->m_languageLabel = new QLabel(tr("Language: "));
        this->m_authorLabel = new QLabel(tr("Author: "));
        this->m_authorLayout->addWidget(this->m_languageLabel);
        this->m_authorLayout->addStretch();
        this->m_authorLayout->addWidget(this->m_authorLabel);
        this->m_authorLayout->addStretch();

        this->m_descriptionGroupBox = new QGroupBox(tr("Description "));
        this->m_descriptionLayout = new QVBoxLayout();
        this->m_descriptionLabel = new QLabel();
        this->m_descriptionLabel->setWordWrap(true);
        this->m_descriptionLayout->addWidget(this->m_descriptionLabel);
        this->m_descriptionLayout->setContentsMargins(0, 0, 0, 0);

        this->m_descriptionGroupBox->setLayout(this->m_descriptionLayout);

        this->m_topLayout->addWidget(this->m_label);
        this->m_topLayout->addLayout(this->m_authorLayout);
        this->m_topLayout->addWidget(this->m_descriptionGroupBox);
        this->m_topLayout->addStretch(1);

        this->m_mainLayout->addLayout(this->m_topLayout, 1);
        this->setLayout(this->m_mainLayout);
    }

    G2pInfoWidget::~G2pInfoWidget() = default;

    void G2pInfoWidget::removeWidget() const {
        QLayoutItem *child;
        while ((child = this->m_mainLayout->takeAt(1)) != nullptr) {
            delete child->widget();
            delete child;
        }
    }

    void G2pInfoWidget::setInfo(const QString &g2pId) const {
        const auto g2pSetFactory = LangSetting::ILangSetManager::instance()->g2pSet(g2pId);
        const auto g2pFactory = LangMgr::IG2pManager::instance()->g2p(g2pId);

        m_languageLabel->setText(tr("Language: ") + g2pFactory->displayName());
        m_authorLabel->setText(tr("Author: ") + g2pFactory->author());
        m_descriptionLabel->setText(g2pFactory->description());

        removeWidget();
        const auto g2pConfigWidget = g2pSetFactory->configWidget(g2pFactory->config(), true);
        this->m_mainLayout->addWidget(g2pConfigWidget, 1);

        connect(g2pSetFactory, &LangSetting::IG2pSetFactory::g2pConfigChanged, this,
                &G2pInfoWidget::g2pConfigChanged);
    }
} // LangMgr