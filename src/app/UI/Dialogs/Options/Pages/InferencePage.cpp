#include "InferencePage.h"

#include "Model/AppOptions/AppOptions.h"
#include "Modules/Inference/Utils/DmlUtils.h"
#include "UI/Controls/CardView.h"
#include "UI/Controls/ComboBox.h"
#include "UI/Controls/DividerLine.h"
#include "UI/Controls/LineEdit.h"
#include "UI/Controls/OptionListCard.h"
#include "UI/Controls/OptionsCard.h"
#include "UI/Controls/OptionsCardItem.h"
#include "UI/Dialogs/Base/RestartDialog.h"

#include <QVBoxLayout>

InferencePage::InferencePage(QWidget *parent) : IOptionPage(parent) {
    auto option = appOptions->inference();
    // Device - Execution Provider
    constexpr int epIndexCpu = 0;
    constexpr int epIndexDirectML = 1;
    m_cbExecutionProvider = new ComboBox();
    m_cbExecutionProvider->insertItem(epIndexCpu, "CPU");
    m_cbExecutionProvider->insertItem(epIndexDirectML, "DirectML");
    if (option->executionProvider == "CPU")
        m_cbExecutionProvider->setCurrentIndex(epIndexCpu);
    else if (option->executionProvider == "DirectML")
        m_cbExecutionProvider->setCurrentIndex(epIndexDirectML);
    connect(m_cbExecutionProvider, &ComboBox::currentIndexChanged, this,
            &InferencePage::modifyOption);
    connect(m_cbExecutionProvider, &ComboBox::currentIndexChanged, this, [=] {
        // modifyOption();
        auto message = tr(
            "The settings will take effect after restarting the app. Do you want to restart now?");
        auto dlg = new RestartDialog(message, true, this);
        dlg->show();
    });

    // Device - GPU
    m_cbDeviceList = new ComboBox();
    auto deviceList = DmlUtils::getDirectXGPUs();

    for (const auto &device : std::as_const(deviceList)) {
        auto displayText =
            QStringLiteral("[%1] %2 (%3 GiB)")
                .arg(device.index)
                .arg(device.description)
                .arg(static_cast<double>(device.memory) / (1024 * 1024 * 1024), 0, 'f', 2);
        m_cbDeviceList->addItem(displayText, device.index);
    }
    if (const auto index_ = m_cbDeviceList->findData(option->selectedGpuIndex); index_ >= 0) {
        m_cbDeviceList->setCurrentIndex(index_);
    }
    connect(m_cbDeviceList, &ComboBox::currentIndexChanged, this, &InferencePage::modifyOption);

    // Device
    auto deviceCard = new OptionListCard(tr("Device"));
    deviceCard->addItem(tr("Execution Provider"), tr("App needs a restart to take effect"),
                        m_cbExecutionProvider);
    deviceCard->addItem(tr("GPU"), m_cbDeviceList);

    // Render - Sampling Steps
    m_cbSamplingSteps = new ComboBox();
    m_cbSamplingSteps->setEditable(true);
    m_cbSamplingSteps->setFixedWidth(80);
    // m_cbSamplingSteps->setStyleSheet("padding-left:0;margin-left:0"); // avoid leading spacing
    m_cbSamplingSteps->setValidator(new QIntValidator(1, 1000));
    m_cbSamplingSteps->addItems({"1", "5", "10", "20", "50", "100"});
    m_cbSamplingSteps->setCurrentText(QString::number(option->samplingSteps));
    connect(m_cbSamplingSteps, &ComboBox::currentTextChanged, this, &InferencePage::modifyOption);

    // Render - Depth
    auto doubleValidator = new QDoubleValidator();
    doubleValidator->setRange(0.0, 1.0);
    m_leDsDepth = new LineEdit(QString::number(option->depth));
    m_leDsDepth->setValidator(doubleValidator);
    m_leDsDepth->setFixedWidth(80);
    connect(m_leDsDepth, &LineEdit::editingFinished, this, &InferencePage::modifyOption);

    auto renderCard = new OptionListCard(tr("Render"));
    renderCard->addItem(tr("Sampling Steps"), m_cbSamplingSteps);
    renderCard->addItem(tr("Depth"), m_leDsDepth);

    // Main Layout
    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(deviceCard);
    mainLayout->addWidget(renderCard);
    mainLayout->addStretch();
    mainLayout->setContentsMargins({});
    setLayout(mainLayout);
}

// InferencePage::~InferencePage() {
//     InferencePage::modifyOption();
// }

void InferencePage::modifyOption() {
    auto option = appOptions->inference();
    option->executionProvider = m_cbExecutionProvider->currentText();
    option->selectedGpuIndex = m_cbDeviceList->currentData().toInt();
    option->samplingSteps = m_cbSamplingSteps->currentText().toInt();
    option->depth = m_leDsDepth->text().toDouble();
    appOptions->saveAndNotify();
}