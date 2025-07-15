#ifndef POSITIONTARGET_H
#define POSITIONTARGET_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

class PositionTarget
{
public:
    PositionTarget(QGraphicsScene *scene, QSizeF size, QPointF position, qreal scale);
    void setPosition(QPointF position);
    void setScale(qreal scale);
    void removeFromScene();
    void updateSceneSize(QSizeF size);
    ~PositionTarget();

private:
    QGraphicsRectItem *vLineRect = nullptr;
    QGraphicsRectItem *hLineRect = nullptr;

    QPointF position;

    QSizeF sceneSize;

    qreal lineWidth = 1;
    qreal scale = 6;

    QPen linePen = QPen(QColor(10, 10, 10, 128), lineWidth * scale);
    QBrush lineBrush = QBrush(QColor(0, 255, 0, 128));
};

#endif // POSITIONTARGET_H
