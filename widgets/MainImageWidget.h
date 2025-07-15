#ifndef MAINIMAGEWIDGET_H
#define MAINIMAGEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>

#include "SelectionRect.h"
#include "SelectionPopup.h"
#include "ImageData.h"

#define MAIN_IMG_MAX_ZOOM 20
#define MAIN_IMG_MIN_ZOOM 1
#define MAIN_IMG_MIN_PIX_DENSITY 10 // Real pixels per 1 pixmap pixel when zoommed in.

class ImageWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = nullptr);

    void setImageData(ImageData *imgData);
    void setZoom(double newZoomLevel);
    void zoomIn();
    void zoomOut();
    void zoomToExtent();
    void clear();
    bool isInitialized();
    ~ImageWidget();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void checkZoom();
    void updateRect();
    void activateRectByPoint(QPointF point);
    void activateRect(SelectionRect * rect);
    void paintEvent(QPaintEvent *event) override;

private:
    ImageData *imageData = nullptr;

    QGraphicsScene *scene = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;
    double zoomLevel = 1.0;
    const double zoomStep = 1.15;
    bool resized = false;
    bool initialized = false;
    bool updateZoomAfterNextRefresh = false;
    bool selectionResizeStarted = false;
    SelectionPopup * selectionPopup = nullptr;
    QGraphicsProxyWidget *selectionPopupProxy = nullptr;

    SelectionRect *currentRect = nullptr;
    QPointF startPos;
    QPointF latestPressPos;
    QPixmap pixmap;

    void selectionPopupDelete();
    void createSelectionPopup();
    bool forwardMouseEvent(QMouseEvent *event);
    SelectionRect * getRectByPoint(QPointF point);

    QMetaObject::Connection imageDataDeleteConnection;

signals:
    void onMousePosChanged(QPointF position);
    void onSelectionChanged(SelectionRect * selection);
};

#endif // MAINIMAGEWIDGET_H
