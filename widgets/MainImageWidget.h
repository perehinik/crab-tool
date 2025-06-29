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

#define MAIN_IMG_MAX_ZOOM 20
#define MAIN_IMG_MIN_ZOOM 1
#define MAIN_IMG_MIN_PIX_DENSITY 10 // Real pixels per 1 pixmap pixel when zoommed in.

class ImageWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = nullptr, const QString imagePath = nullptr);

    QString imagePath;

    void setImage(QString imagePath);
    void setZoom(double newZoomLevel);
    void zoomIn();
    void zoomOut();
    void zoomToExtent();
    void clear();
    QJsonObject toJson();
    void fromJson(const QJsonObject &json);
    int selectionCount();
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

private:
    QGraphicsScene *scene = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;
    double zoomLevel = 1.0;
    const double zoomStep = 1.15;
    bool resized = false;
    bool initialized = false;
    bool selectionResizeStarted = false;
    SelectionPopup * selectionPopup = nullptr;
    QGraphicsProxyWidget *selectionPopupProxy = nullptr;

    QList<SelectionRect*> rectangleList;
    SelectionRect *currentRect = nullptr;
    QPointF startPos;
    QPointF latestPressPos;
    QPixmap pixmap;

    QPen rectanglePen = QPen(Qt::green, 20);

    void selectionPopupDelete();
    void createSelectionPopup();
    bool forwardMouseEvent(QMouseEvent *event);
    SelectionRect * getRectByPoint(QPointF point);

signals:
    void onMousePosChanged(QPointF position);
    void onSelectionChanged(SelectionRect * selection);
};

#endif // MAINIMAGEWIDGET_H
