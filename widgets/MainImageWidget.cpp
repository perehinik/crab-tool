#include <QPalette>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QMessageBox>

#include "MainImageWidget.h"

ImageWidget::ImageWidget(QWidget *parent) : QGraphicsView(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(300);
    setStyleSheet("border: solid lightgrey; border-width: 0px 1px 0px 1px;");

    scene = new QGraphicsScene(this);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setMouseTracking(true);
}

bool ImageWidget::isInitialized() {
    return initialized;
}

void ImageWidget::clear() {
    selectionPopup = nullptr;
    selectionPopupProxy = nullptr;
    activateRect(nullptr);
    updateZoomAfterNextRefresh = false;
    initialized = false;
    if (imageData) {
        imageData->removeFromScene();
    }
    disconnect(imageDataDeleteConnection);
    imageData = nullptr;
    if (imageItem) {
        imageItem->setVisible(false);
    }
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

void ImageWidget::setImageData(ImageData *imgData) {
    clear();
    // This allows to remove ImageData pointer before object is destroyed by ProjectData
    imageDataDeleteConnection = connect(imgData, &ImageData::aboutToBeDeleted, this, &ImageWidget::clear);
    imageData = imgData;
    createSelectionPopup();
    pixmap = QPixmap(imageData->imagePath);
    if (pixmap.isNull()) {
        QMessageBox::critical(this, "Error", "Image loading error!\nFile: " + imageData->imagePath);
    } else {
        if (!imageItem) {
            imageItem = scene->addPixmap(pixmap);
        } else {
            imageItem->setPixmap(pixmap);
            imageItem->setPos(0, 0);
        }
        imageItem->setVisible(true);
        scene->setSceneRect(pixmap.rect());
        imageData->addToScene(scene);
        updateZoomAfterNextRefresh = true;
        initialized = true;
    }
    zoomToExtent();
}

void ImageWidget::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);
    if (updateZoomAfterNextRefresh && imageData) {
        updateZoomAfterNextRefresh = false;
        imageData->setScale(1.0 / transform().m11());
    }
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
    if (imageData) {
        imageData->setScale(1 / transform().m11());
    }
}

void ImageWidget::setZoom(double newZoomLevel) {
    if (!pixmap || pixmap.isNull()) {
        return;
    }
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
    if (imageData) {
        imageData->setScale(1 / transform().m11());
    }
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
    if (!initialized || forwardMouseEvent(event) || !imageData) {
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
            SelectionRect * rect = new SelectionRect(QRectF(startPos, startPos));
            rect->addToScene(scene);
            rect->setScale(1 / transform().m11());
            imageData->appendSelection(rect);
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
    if (!initialized || forwardMouseEvent(event) || !imageData) {
        return;
    }
    if (event->button() == Qt::LeftButton && currentRect && selectionResizeStarted) {
        // ToDo: Check if rect is too small to add
        if (!imageData->containsSelection(currentRect) ) {
            imageData->appendSelection(currentRect);
        }
        if (currentRect->getVisibleArea() < 60) {
            imageData->deleteRect(currentRect);
            currentRect = nullptr;
            activateRect(nullptr);
        }
        selectionResizeStarted = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

SelectionRect * ImageWidget::getRectByPoint(QPointF point) {
    QPointF *preActivatedPoint = nullptr;
    SelectionRect * activatedRect = nullptr;
    qreal distance = 9999;

    for (int i = 0; i < imageData->rectangleList.size(); ++i) {
        preActivatedPoint = imageData->rectangleList[i]->getCornerPoint(point);
        if (!preActivatedPoint) {
            continue;
        } else if (!activatedRect || QLineF(*preActivatedPoint, point).length() < distance) {
            activatedRect = imageData->rectangleList[i];
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
    if (!imageData) {
        return;
    }
    if (currentRect && imageData->containsSelection(currentRect)) {
        currentRect->deactivate();
    }
    if (rect && imageData->containsSelection(rect)) {
        currentRect = rect;
        rect->activate();
    } else {
        currentRect = nullptr;
    }
    emit onSelectionChanged(rect);
}

void ImageWidget::selectionPopupDelete() {
    SelectionRect * activatedRect = getRectByPoint(latestPressPos);
    imageData->deleteRect(activatedRect);
    activatedRect = nullptr;
    activateRect(nullptr);
    selectionPopupProxy->setVisible(false);
}

ImageWidget::~ImageWidget() {
    clear();
}
