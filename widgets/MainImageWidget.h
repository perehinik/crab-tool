#ifndef MAINIMAGEWIDGET_H
#define MAINIMAGEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>

#include "SelectionRect.h"

class ImageWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = nullptr, const QString imagePath = nullptr);
    void setImage(QString imagePath);
    void setZoom(double factor, double newZoomLevel);
    void zoomIn();
    void zoomOut();
    void zoomToExtent();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void checkZoom();
    void updateRect();
    int activateRectByPoint(QPointF point);
    void setResized(bool isResized);

private:
    QString imagePath;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;
    double zoomLevel = 1.0;
    const double zoomStep = 1.15;
    const double maxZoom = 100.0;
    bool resized = false;
    bool initialized = false;

    QList<SelectionRect*> rectangleList;
    SelectionRect *currentRect = nullptr;
    QPointF startPos;
    QPixmap pixmap;

    QPen rectanglePen = QPen(Qt::green, 20);
};

#endif // MAINIMAGEWIDGET_H
