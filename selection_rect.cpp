    #include "selection_rect.h"



SelectionRect::SelectionRect(QGraphicsScene *scene, const QRectF rect, qreal scale)
{
    this->scale = scale;
    graphicsRect = scene->addRect(rect, linePen);
    corners = {
        rect.topLeft(), rect.topRight(),
        rect.bottomLeft(), rect.bottomRight()
    };
    for (int i = 0; i < corners.size(); i++) {
        QPointF corner = corners[i];
        qreal circleSizeScaled = circleSize * scale;

        QRectF circleRect(
            corner.x() - (circleSizeScaled / 2),
            corner.y() - (circleSizeScaled / 2),
            circleSizeScaled,
            circleSizeScaled
            );

        QGraphicsEllipseItem *ellipse = scene->addEllipse(circleRect, circlePen, circleBrush);
        ellipses.append(ellipse);
    }
    // startPos = mapToScene(event->pos());
    // currentRect = scene->addRect(QRectF(startPos, startPos), rectanglePen);
}

void SelectionRect::removeFromScene(QGraphicsScene *scene)
{
    scene->removeItem(graphicsRect);
    for (int i = 0; i < ellipses.size(); i++) {
        scene->removeItem(ellipses[i]);
    }
}

void SelectionRect::setRect(QRectF rect)
{
    // Update rectangle
    graphicsRect->setRect(rect);
    // Update circles
    corners = {
        rect.topLeft(), rect.topRight(),
        rect.bottomLeft(), rect.bottomRight()
    };

    for (int i = 0; i < ellipses.size(); i++) {
        QPointF corner = corners[i];
        qreal circleSizeScaled = circleSize * scale;
        QRectF circleRect(corner.x() - (circleSizeScaled / 2), corner.y() - (circleSizeScaled / 2), circleSizeScaled, circleSizeScaled);
        ellipses[i]->setRect(circleRect);
    }
}

QRectF SelectionRect::getRect()
{
    return graphicsRect->rect();
}

void SelectionRect::setScale(qreal scale)
{
    this->scale = scale;
    setRect(graphicsRect->rect());
    linePen.setWidth(lineWidth * scale);
    graphicsRect->setPen(linePen);
}

QPointF *SelectionRect::getCornerPoint(QPointF point)
{
    for (int i = 0; i < corners.size(); i++) {
        if (abs(QLineF(corners[i], point).length()) < circleSize / 2 * scale) {
            return &corners[i];
        }
    }
    return nullptr;
}

QPointF *SelectionRect::getOppositePoint(QPointF point)
{
    for (int i = 0; i < corners.size(); i++) {
        if (std::round(corners[i].x() * 10000) != std::round(point.x() * 10000) &&
            std::round(corners[i].y() * 10000) != std::round(point.y() * 10000)) {
            return &corners[i];
        }
    }
    // In case rectangle is forms a line
    for (int i = 0; i < corners.size(); i++) {
        if (std::round(corners[i].x() * 10000) != std::round(point.x() * 10000) ||
            std::round(corners[i].y() * 10000) != std::round(point.y() * 10000)) {
            return &corners[i];
        }
    }
    // In case all points in rectangle are same
    return &corners[0];
}

qreal SelectionRect::getVisibleArea()
{
    QRectF rect = getRect();
    qreal vertical = QLineF(rect.topLeft(), rect.topRight()).length() / scale;
    qreal horisontal = QLineF(rect.topLeft(), rect.bottomLeft()).length() / scale;
    return vertical * horisontal;
}
