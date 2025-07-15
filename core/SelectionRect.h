#ifndef SELECTIONRECT_H
#define SELECTIONRECT_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

class SelectionRect : public QObject {
    Q_OBJECT

public:
    SelectionRect(const QRectF rect);
    SelectionRect(const QJsonObject &json);
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

    QJsonObject toJson();
    void fromJson(const QJsonObject &json);
    void addToScene(QGraphicsScene *scene);
    ~SelectionRect();

private:
    QRectF rect;
    QList<QPointF> corners;
    QGraphicsRectItem *graphicsRect = nullptr;
    QList<QGraphicsEllipseItem*> ellipses;

    qreal lineWidth = 2;
    qreal circleSize = 8;
    qreal scale = 6;

    QPen linePen = QPen(QColor(0, 255, 0, 128), lineWidth * scale);
    QPen circlePen = QPen(QColor(220, 220, 220, 100), 0);
    QBrush circleBrush = QBrush(QColor(20, 60, 100, 200));

    QPen circlePenActive = QPen(QColor(255, 0, 0, 255), 0);
    QBrush circleBrushActive = QBrush(QColor(255, 0, 0, 255));

    void buildCornerEllipses(QGraphicsScene *scene);
    void updateGraphicsItems();

signals:
    void aboutToBeDeleted();
};

#endif // SELECTIONRECT_H
