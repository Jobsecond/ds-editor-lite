//
// Created by fluty on 24-3-16.
//

#include "AppearancePage.h"

#include <QVBoxLayout>

#include "Model/AppOptions/AppOptions.h"
#include "UI/Controls/ComboBox.h"
#include "UI/Controls/LineEdit.h"
#include "Controller/AppOptionsController.h"

AppearancePage::AppearancePage(QWidget *parent) : QWidget(parent) {
    auto option = AppOptions::instance()->appearance();
    m_cbxAnimationLevel = new ComboBox;
    m_cbxAnimationLevel->addItems(animationLevelsName);
    m_cbxAnimationLevel->setCurrentIndex(option->animationLevel);
    connect(m_cbxAnimationLevel, &ComboBox::currentIndexChanged, this,
            &AppearancePage::modifyOptions);

    m_leAnimationTimeScale = new LineEdit;
    auto doubleValidator = new QDoubleValidator(m_leAnimationTimeScale);
    m_leAnimationTimeScale->setValidator(doubleValidator);
    m_leAnimationTimeScale->setText(QString::number(option->animationTimeScale));
    connect(m_leAnimationTimeScale, &LineEdit::editingFinished, this,
            &AppearancePage::modifyOptions);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_cbxAnimationLevel);
    mainLayout->addWidget(m_leAnimationTimeScale);
    setLayout(mainLayout);
}
void AppearancePage::modifyOptions() {
    AppearanceOption option;
    option.animationLevel = static_cast<AnimationGlobal::AnimationLevels>(m_cbxAnimationLevel->currentIndex());
    option.animationTimeScale = m_leAnimationTimeScale->text().toDouble();
    auto controller = AppOptionsController::instance()->appearanceController();
    controller->setOption(option);
}