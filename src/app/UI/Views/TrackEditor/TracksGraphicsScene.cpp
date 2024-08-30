//
// Created by fluty on 2023/11/14.
//

#include "TracksGraphicsScene.h"

#include "Global/TracksEditorGlobal.h"

TracksGraphicsScene::TracksGraphicsScene() {
    setPixelsPerQuarterNote(TracksEditorGlobal::pixelsPerQuarterNote);
}

int TracksGraphicsScene::trackIndexAt(double sceneY) const {
    auto yToTrackIndex = [&](double y) { return static_cast<int>(y / (TracksEditorGlobal::trackHeight * scaleY())); };
    auto index = yToTrackIndex(sceneY);
    if (index >= m_trackCount)
        index = -1;
    return index;
}

int TracksGraphicsScene::tickAt(double sceneX) const {
    return static_cast<int>(480 * sceneX / scaleX() / TracksEditorGlobal::pixelsPerQuarterNote);
}

void TracksGraphicsScene::onViewResized(QSize size) {
    m_graphicsViewSize = size;
    updateSceneRect();
}

void TracksGraphicsScene::onTrackCountChanged(int count) {
    m_trackCount = count;
    updateSceneRect();
}

void TracksGraphicsScene::updateSceneRect() {
    // CommonGraphicsScene::updateSceneRect();
    auto targetSceneWidth = sceneBaseSize().width() * scaleX();
    // auto targetSceneHeight = sceneSize().height() * scaleY();
    auto totalTrackHeight = m_trackCount * TracksEditorGlobal::trackHeight * scaleY();
    auto viewHeight = m_graphicsViewSize.height();

    auto targetSceneHeight = totalTrackHeight;
    // Adjust scene height to match view when track count or scaleY is too small
    if (totalTrackHeight < viewHeight) {
        // auto paddingBottom = viewHeight - totalTrackHeight;
        targetSceneHeight = viewHeight;
    }
    setSceneRect(0, 0, targetSceneWidth, targetSceneHeight);
}