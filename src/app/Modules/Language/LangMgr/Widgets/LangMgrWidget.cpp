#include "LangMgrWidget.h"

namespace LangMgr {
    LangMgrWidget::LangMgrWidget(QWidget *parent) : QWidget(parent) {
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);

        m_tableLayout = new QVBoxLayout();
        m_tableLayout->setContentsMargins(0, 0, 0, 0);

        m_langListWidget = new LangListWidget(this);

        m_buttonLayout = new QHBoxLayout();
        m_buttonLayout->setContentsMargins(0, 0, 0, 0);
        m_applyButton = new Button(tr("Apply"), this);
        m_buttonLayout->addStretch(1);
        m_buttonLayout->addWidget(m_applyButton);

        m_centerLayout = new QHBoxLayout();
        m_centerLayout->setContentsMargins(0, 0, 0, 0);

        m_langInfoWidget = new LangInfoWidget(this);

        m_centerLayout->addWidget(m_langListWidget, 2);
        m_centerLayout->addWidget(m_langInfoWidget, 3);

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

        // 监控表格选中行的变化
        connect(m_langListWidget, &QListWidget::currentRowChanged, m_langInfoWidget,
                [this] { m_langInfoWidget->setInfo(m_langListWidget->currentConfig()); });
    }

    LangMgrWidget::~LangMgrWidget() = default;
} // LangMgr