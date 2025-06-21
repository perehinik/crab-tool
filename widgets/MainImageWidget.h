#ifndef MAINIMAGEWIDGET_H
#define MAINIMAGEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>

#include "SelectionRect.h"

#define MAIN_IMG_MAX_ZOOM 20
#define MAIN_IMG_MIN_ZOOM 1
#define MAIN_IMG_MIN_PIX_DENSITY 10 // Real pixels per 1 pixmap pixel when zoommed in.

class ImageWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = nullptr, const QString imagePath = nullptr);
    void setImage(QString imagePath);
    void setZoom(double newZoomLevel);
    void zoomIn();
    void zoomOut();
    void zoomToExtent();
    void clear();
    ~ImageWidget();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void checkZoom();
    void updateRect();
    int activateRectByPoint(QPointF point);

private:
    QString imagePath;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;
    double zoomLevel = 1.0;
    const double zoomStep = 1.15;
    bool resized = false;
    bool initialized = false;

    QList<SelectionRect*> rectangleList;
    SelectionRect *currentRect = nullptr;
    QPointF startPos;
    QPixmap pixmap;

    QPen rectanglePen = QPen(Qt::green, 20);

signals:
    void onMousePosChanged(QPointF position);
};

#endif // MAINIMAGEWIDGET_H
