#ifndef SELECTIONRECT_H
#define SELECTIONRECT_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

class SelectionRect
{
public:
    SelectionRect(QGraphicsScene *scene, const QRectF rect, qreal scale = 6);
    SelectionRect(QGraphicsScene *scene, const QJsonObject &json, qreal scale = 6);
    QStringList tags;
    QPointF *getCornerPoint(QPointF point);
    QPointF *getOppositePoint(QPointF point);

    void setRect(QRectF rect);
    void setScale(qreal scale);
    QRectF getRect();
    qreal getVisibleArea();
    void removeFromScene();
    void deactivate();
    void activate();
    void buildCorners(QGraphicsScene *scene, QRectF rect);
    QJsonObject toJson();
    void fromJson(const QJsonObject &json);
    ~SelectionRect();

private:
    QGraphicsRectItem *graphicsRect = nullptr;
    QList<QPointF> corners;
    QList<QGraphicsEllipseItem*> ellipses;

    QPointF startPos;

    qreal lineWidth = 2;
    qreal circleSize = 8;
    qreal scale = 6;

    QPen linePen = QPen(QColor(0, 255, 0, 128), lineWidth * scale);
    QPen circlePen = QPen(QColor(220, 220, 220, 100), 0);
    QBrush circleBrush = QBrush(QColor(20, 60, 100, 200));

    QPen circlePenActive = QPen(QColor(255, 0, 0, 255), 0);
    QBrush circleBrushActive = QBrush(QColor(255, 0, 0, 255));
};

#endif // SELECTIONRECT_H
