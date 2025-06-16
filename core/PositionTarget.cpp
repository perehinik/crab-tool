#include "PositionTarget.h"

PositionTarget::PositionTarget(QGraphicsScene *scene, QSizeF size, QPointF position, qreal scale) {
    this->scale = scale;
    sceneSize = size;
    vLineRect = scene->addRect(position.x() - 1, 0, 1, sceneSize.height(), linePen, lineBrush);
    hLineRect = scene->addRect(0, position.y() - 1, sceneSize.width(), 1, linePen, lineBrush);
}

void PositionTarget::updateSceneSize(QSizeF size) {
    sceneSize = size;
    setPosition(position);
}

void PositionTarget::removeFromScene() {
    if (vLineRect && vLineRect->scene()) {
        vLineRect->scene()->removeItem(vLineRect);
    }
    if (hLineRect && hLineRect->scene()) {
        hLineRect->scene()->removeItem(hLineRect);
    }
}

void PositionTarget::setPosition(QPointF pos) {
    position = pos;
    vLineRect->setRect(position.x() - 1, 0, 1.5 * scale, sceneSize.height());
    hLineRect->setRect(0, position.y() - 1, sceneSize.width(), 1.5 * scale);
}

void PositionTarget::setScale(qreal scale) {
    qDebug() << "scale:" << scale;
    this->scale = scale;
    linePen.setWidthF(lineWidth * scale);
    vLineRect->setPen(linePen);
    hLineRect->setPen(linePen);
    setPosition(position);
}

PositionTarget::~PositionTarget() {
    removeFromScene();
}
