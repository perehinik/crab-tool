#ifndef IMAGEZOOMWIDGET_H
#define IMAGEZOOMWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGridLayout>
#include <QSlider>

#include "ToolboxButtonWidget.h"
#include "PositionTarget.h"

#define MIN_PIXEL_DENSITY 20.0

class ImageZoomWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImageZoomWidget(QWidget *parent = nullptr);
    void setImage(QString imagePath);
    void setZoom(double newZoomLevel);
    void setPosition(QPointF position);
    void centerOn(QPointF position);

private:
    QString imagePath;
    QGridLayout *layout;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;
    double zoomLevel = 1.0;
    bool initialized = false;
    QPixmap pixmap;
    ToolboxButton *zoomOutButton;
    ToolboxButton *zoomInButton;
    QSlider *slider;
    PositionTarget *positionTarget = nullptr;

    void resizeEvent(QResizeEvent *event) override;
    void onSliderValueChanged(int value);
    void zoomIn();
    void zoomOut();
    void zoomToExtent();
    void updatePositionTarget(QPointF pos, qreal targetScale = 0);
};

#endif // IMAGEZOOMWIDGET_H
