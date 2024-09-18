//
// Created by fluty on 24-8-21.
//

#include "ParamEditorView.h"

#include "ParamEditorGraphicsScene.h"
#include "ParamEditorGraphicsView.h"
#include "UI/Views/ClipEditor/ClipEditorGlobal.h"

#include "Model/AppModel/SingingClip.h"

#include <QVBoxLayout>

ParamEditorView::ParamEditorView(QWidget *parent) : QWidget(parent) {
    auto scene = new ParamEditorGraphicsScene;
    m_graphicsView = new ParamEditorGraphicsView(scene, this);
    connect(m_graphicsView, &ParamEditorGraphicsView::sizeChanged, scene,
            &ParamEditorGraphicsScene::onViewResized);

    auto layout = new QHBoxLayout;
    layout->addSpacing(ClipEditorGlobal::pianoKeyboardWidth);
    layout->addWidget(m_graphicsView);
    // layout->setContentsMargins(0, 0, 0, 6);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    setMinimumHeight(128);
}

void ParamEditorView::setDataContext(SingingClip *clip) const {
    m_graphicsView->setDataContext(clip);
}

ParamEditorGraphicsView *ParamEditorView::graphicsView() const {
    return m_graphicsView;
}

void ParamEditorView::onParamChanged(ParamInfo::Name name, Param::Type type) const {
    auto param = m_clip->params.getParamByName(name);
    if (m_foregroundParam == name) {
        m_graphicsView->setForegroundParam(type, *param);
    } else if (m_backgroundParam == name)
        m_graphicsView->setBackgroundParam(type, *param);
}