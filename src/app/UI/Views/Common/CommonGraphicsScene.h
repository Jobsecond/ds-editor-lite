//
// Created by fluty on 2024/1/23.
//

#ifndef COMMONGRAPHICSSCENE_H
#define COMMONGRAPHICSSCENE_H

#include <QGraphicsScene>

#include "UI/Utils/IScalableItem.h"


class CommonGraphicsLayer;
class IScalableItem;
class CommonGraphicsRectItem;

class CommonGraphicsScene : public QGraphicsScene, public IScalableItem {
public:
    explicit CommonGraphicsScene(QObject *parent = nullptr);
    ~CommonGraphicsScene() override = default;
    [[nodiscard]] QSizeF sceneSize() const;
    void setSceneSize(const QSizeF &size);
    void addCommonItem(IScalableItem *item);
    void removeCommonItem(IScalableItem *item);

protected:
    virtual void updateSceneRect();
    void afterSetScale() override;
    void afterSetVisibleRect() override;

private:
    using QGraphicsScene::addItem;
    using QGraphicsScene::removeItem;
    QSizeF m_sceneSize = QSizeF(1920, 1080);
    QList<IScalableItem *> m_items;
};

#endif // COMMONGRAPHICSSCENE_H
