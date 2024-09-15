//
// Created by fluty on 24-8-21.
//

#include "ParamEditorGraphicsView.h"

#include "ParamEditorGraphicsScene.h"
#include "UI/Views/ClipEditor/ClipEditorGlobal.h"
#include "UI/Views/ClipEditor/CommonParamEditorView.h"
#include "UI/Views/Common/TimeGridGraphicsItem.h"

ParamEditorGraphicsView::ParamEditorGraphicsView(ParamEditorGraphicsScene *scene,QWidget *parent)
    : TimeGraphicsView(scene, parent) {
    setAttribute(Qt::WA_StyledBackground);
    setPixelsPerQuarterNote(ClipEditorGlobal::pixelsPerQuarterNote);
    setMinimumHeight(0);
    setScrollBarVisibility(Qt::Horizontal, false);
    setScrollBarVisibility(Qt::Vertical, false);

    auto grid = new TimeGridGraphicsItem;
    grid->setPixelsPerQuarterNote(ClipEditorGlobal::pixelsPerQuarterNote);
    setGridItem(grid);

    auto editor = new CommonParamEditorView;
    editor->setZValue(2);
    editor->setTransparentMouseEvents(false);
    scene->addCommonItem(editor);
}