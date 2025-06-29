#include <QPalette>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>

#include "MainImageWidget.h"

ImageWidget::ImageWidget(QWidget *parent, QString imagePath) : QGraphicsView(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(300);
    setStyleSheet("border: solid lightgrey; border-width: 0px 1px 0px 1px;");

    scene = new QGraphicsScene(this);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setImage(imagePath);

    createSelectionPopup();

    setMouseTracking(true);
}

QJsonObject ImageWidget::toJson() {
    QJsonObject obj;

    obj["type"] = "image-data";
    obj["image-path"] = imagePath;
    obj["image-filename"] = QFileInfo(imagePath).fileName();

    QJsonArray rectArray;
    for (int i = 0; i < rectangleList.length(); ++i) {
        if (rectangleList[i]) {
            rectArray.append(rectangleList[i]->toJson());
        }
    }
    obj["selection-list"] = rectArray;
    return obj;
}

void ImageWidget::fromJson(const QJsonObject &json) {
    if (json["type"].toString() != "image-data") {
        return;
    }
    if (json["image-path"].toString() != imagePath) {
        setImage(json["image-path"].toString());
    }
    QJsonArray rectArray = json["selection-list"].toArray();
    for (int i = 0; i < rectArray.size(); ++i) {
        QJsonObject jsonObj = rectArray.at(i).toObject();
        rectangleList.append(new SelectionRect(scene, jsonObj, (qreal)1.0 / transform().m11()));
    }
}

void ImageWidget::clear() {
    for (int i = 0; i < rectangleList.length(); i++) {
        delete rectangleList[i];
    }
    scene->clear();
    rectangleList.clear();
    selectionPopup = nullptr;
    selectionPopupProxy = nullptr;
    activateRect(nullptr);
    initialized = false;
}

void ImageWidget::createSelectionPopup() {
    if (selectionPopup || !scene) {
        return;
    }
    selectionPopup = new SelectionPopup();
    selectionPopupProxy = scene->addWidget(selectionPopup);
    selectionPopupProxy->setZValue(100);
    selectionPopupProxy->setVisible(false);
    connect(selectionPopup, &SelectionPopup::onDeleteClicked, this, &ImageWidget::selectionPopupDelete);
}

bool ImageWidget::forwardMouseEvent(QMouseEvent *event) {
    QPointF scenePos = mapToScene(event->pos());
    if (selectionPopupProxy && selectionPopupProxy->isVisible() &&
        selectionPopupProxy->sceneBoundingRect().contains(scenePos)) {

        QPointF popupLocal = selectionPopupProxy->mapFromScene(scenePos);
        QMouseEvent forwardedEvent(
            event->type(),
            popupLocal,
            event->position(),
            event->globalPosition(),
            event->button(),
            event->buttons(),
            event->modifiers()
        );

        QWidget *target = selectionPopup->childAt(popupLocal.toPoint());
        if (!target) target = selectionPopup;

        QApplication::sendEvent(target, &forwardedEvent);
        return true;
    }
    return false;
}

void ImageWidget::setImage(QString imagePath) {
    clear();
    createSelectionPopup();
    pixmap = QPixmap(imagePath);
    if (!pixmap.isNull()) {
        imageItem = scene->addPixmap(pixmap);
        // imageItem->setTransformationMode(Qt::SmoothTransformation);
        imageItem->setPos(0, 0);
        scene->setSceneRect(pixmap.rect());
        this->imagePath = imagePath;
        initialized = true;
    }
    zoomToExtent();
}

void ImageWidget::checkZoom() {
    if (!scene) {
        return;
    }
    // Prevent zooming out beyond full-scene view
    QRectF viewBounds = mapToScene(viewport()->rect()).boundingRect();
    QRectF sceneBounds = scene->sceneRect();

    if (transform().m11() < 2) {
        // Use better transformation for zoomed out to get better image
        imageItem->setTransformationMode(Qt::SmoothTransformation);
    } else {
        // Use fast for zoomed image so pixels are visible
        imageItem->setTransformationMode(Qt::FastTransformation);
    }

    if (viewBounds.contains(sceneBounds)) {
        zoomToExtent();
    }
}

void ImageWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    if (!initialized) {
        return;
    }
    if (scene && scene->sceneRect().isValid() && !resized) {
        zoomToExtent();
    }
    checkZoom();
    updateRect();
}

void ImageWidget::setZoom(double newZoomLevel) {
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    newZoomLevel = newZoomLevel > MAIN_IMG_MAX_ZOOM ? MAIN_IMG_MAX_ZOOM : newZoomLevel;
    newZoomLevel = newZoomLevel < MAIN_IMG_MIN_ZOOM ? MAIN_IMG_MIN_ZOOM : newZoomLevel;
    zoomLevel = newZoomLevel;
    if (std::round(zoomLevel * 1000) == 1000) {
        this->resized = false;
        return;
    }
    this->resized = true;
    // P5ixel density divided by miniumum transformation
    qreal scaleMax = MAIN_IMG_MIN_PIX_DENSITY / transform().m11();
    // Calculate new scale using logarithmic interpolation
    qreal newScale = std::pow(scaleMax, (newZoomLevel - MAIN_IMG_MIN_ZOOM) / (MAIN_IMG_MAX_ZOOM - MAIN_IMG_MIN_ZOOM));
    // Calculate new scale using linear interpolation
    //qreal newScale = 1 + (newZoomLevel - MAIN_IMG_MIN_ZOOM) * (scaleMax - 1) / (MAIN_IMG_MAX_ZOOM - MAIN_IMG_MIN_ZOOM);

    scale(newScale, newScale);

    // Prevent zooming out beyond full-scene view
    checkZoom();
    updateRect();
}

void ImageWidget::zoomIn() {
    setZoom(zoomLevel + 1);
}

void ImageWidget::zoomOut() {
    setZoom(zoomLevel - 1);
}

void ImageWidget::zoomToExtent() {
    setZoom(1);
}

void ImageWidget::wheelEvent(QWheelEvent *event) {
    if (!initialized || event->angleDelta().x() != 0) {
        return;
    }
    // Get position in scene coordinates under cursor
    QPointF cursorScenePos = mapToScene(event->position().toPoint());

    // Determine zoom direction
    bool zoomInEvent = event->angleDelta().y() > 0;
    zoomInEvent ? zoomIn() : zoomOut();

    // Keep mouse cursor fixed on the same scene point
    QPointF newCursorViewPos = mapFromScene(cursorScenePos);
    QPointF viewCenterShift = newCursorViewPos - event->position();
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + viewCenterShift.x());
    verticalScrollBar()->setValue(verticalScrollBar()->value() + viewCenterShift.y());
}

void ImageWidget::mousePressEvent(QMouseEvent *event) {
    if (!initialized || forwardMouseEvent(event)) {
        return;
    }

    latestPressPos = mapToScene(event->pos());
    QSize imageSize = imageItem->pixmap().size();
    int imageWidth = imageSize.width();
    int imageHeight = imageSize.height();

    if (latestPressPos.x() < 0 ||
        latestPressPos.y() < 0 ||
        latestPressPos.x() > imageWidth ||
        latestPressPos.y() > imageHeight) {
            return;
    }

    if (event->button() == Qt::LeftButton && dragMode() == QGraphicsView::NoDrag) {
        activateRectByPoint(latestPressPos);
        if (!currentRect) {
            startPos = latestPressPos;
            SelectionRect * rect = new SelectionRect(scene, QRectF(startPos, startPos), (qreal)1.0 / transform().m11());
            rect->setScale(1 / transform().m11());
            rectangleList.append(rect);
            activateRect(rect);
        }
        selectionResizeStarted = true;
    }

    if (!selectionPopup) {
        createSelectionPopup();
    }

    selectionPopupProxy->setVisible(false);
    if (event->button() == Qt::RightButton) {
        if (getRectByPoint(latestPressPos)) {
            selectionPopupProxy->setPos(latestPressPos);
            selectionPopupProxy->setScale(1.0 / transform().m11());
            selectionPopupProxy->setVisible(true);
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!initialized || forwardMouseEvent(event)) {
        return;
    }

    QPointF currentPos = mapToScene(event->pos());
    QSize imageSize = imageItem->pixmap().size();
    int imageWidth = imageSize.width();
    int imageHeight = imageSize.height();

    if (currentPos.x() < 0) { currentPos.setX(0); }
    if (currentPos.y() < 0) { currentPos.setY(0); }
    if (currentPos.x() > imageWidth) { currentPos.setX(imageWidth); }
    if (currentPos.y() > imageHeight) { currentPos.setY(imageHeight); }

    if (currentRect && selectionResizeStarted) {
        QRectF newRect(startPos, currentPos);
        currentRect->setRect(newRect.normalized());
        emit onSelectionChanged(currentRect);
    }

    emit onMousePosChanged(currentPos);
    QGraphicsView::mouseMoveEvent(event);
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!initialized || forwardMouseEvent(event)) {
        return;
    }
    if (event->button() == Qt::LeftButton && currentRect && selectionResizeStarted) {
        // ToDo: Check if rect is too small to add
        if (!rectangleList.contains(currentRect) ) {
            rectangleList.append(currentRect);
        }
        if (currentRect->getVisibleArea() < 60) {
            rectangleList.removeAll(currentRect);
            delete currentRect;
            activateRect(nullptr);
        }
        selectionResizeStarted = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageWidget::updateRect() {
    qreal scale = 1 / transform().m11();
    for (int i = 0; i < rectangleList.size(); ++i) {
        rectangleList[i]->setScale(scale);
    }
}

SelectionRect * ImageWidget::getRectByPoint(QPointF point) {
    QPointF *preActivatedPoint = nullptr;
    SelectionRect * activatedRect = nullptr;
    qreal distance = 9999;

    for (int i = 0; i < rectangleList.size(); ++i) {
        preActivatedPoint = rectangleList[i]->getCornerPoint(point);
        if (!preActivatedPoint) {
            continue;
        } else if (!activatedRect || QLineF(*preActivatedPoint, point).length() < distance) {
            activatedRect = rectangleList[i];
            startPos = *activatedRect->getOppositePoint(*preActivatedPoint);
            distance = abs(QLineF(startPos, point).length());
        }
    }
    return activatedRect;
}

void ImageWidget::activateRectByPoint(QPointF point) {
    activateRect(getRectByPoint(point));
}

void ImageWidget::activateRect(SelectionRect * rect) {
    if (currentRect && rectangleList.contains(currentRect)) {
        currentRect->deactivate();
    }
    if (rectangleList.contains(rect)) {
        currentRect = rect;
        rect->activate();
    } else {
        currentRect = nullptr;
    }
    emit onSelectionChanged(rect);
}

void ImageWidget::selectionPopupDelete() {
    SelectionRect * activatedRect = getRectByPoint(latestPressPos);
    rectangleList.removeAll(activatedRect);
    delete activatedRect;
    activateRect(nullptr);
    selectionPopupProxy->setVisible(false);
}

int ImageWidget::selectionCount() {
    return rectangleList.length();
}

ImageWidget::~ImageWidget() {
    clear();
}
