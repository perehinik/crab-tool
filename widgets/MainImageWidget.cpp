#include <QPalette>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QDebug>

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

    setMouseTracking(true);
}

void ImageWidget::setImage(QString imagePath) {
    rectangleList.clear();
    currentRect = nullptr;
    scene->clear();
    initialized = false;
    pixmap = QPixmap(imagePath);
    if (!pixmap.isNull()) {
        imageItem = scene->addPixmap(pixmap);
        // imageItem->setTransformationMode(Qt::SmoothTransformation);
        imageItem->setPos(0, 0);
        scene->setSceneRect(pixmap.rect());
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
    zoomLevel = newZoomLevel;
    if (std::round(zoomLevel * 1000) == 1000) {
        this->resized = false;
        return;
    }
    this->resized = true;
    newZoomLevel = newZoomLevel > MAIN_IMG_MAX_ZOOM ? MAIN_IMG_MAX_ZOOM : newZoomLevel;
    newZoomLevel = newZoomLevel < MAIN_IMG_MIN_ZOOM ? MAIN_IMG_MIN_ZOOM : newZoomLevel;
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
    if (!initialized) {
        return;
    }

    QPointF currentPos = mapToScene(event->pos());
    QSize imageSize = imageItem->pixmap().size();
    int imageWidth = imageSize.width();
    int imageHeight = imageSize.height();

    if (currentPos.x() < 0 ||
        currentPos.y() < 0 ||
        currentPos.x() > imageWidth ||
        currentPos.y() > imageHeight) {
            return;
    }

    if (event->button() == Qt::LeftButton && dragMode() == QGraphicsView::NoDrag) {
        activateRectByPoint(currentPos);
        if (!currentRect) {
            startPos = currentPos;
            currentRect = new SelectionRect(scene, QRectF(startPos, startPos), (qreal)1.0 / transform().m11());
            currentRect->setScale(1 / transform().m11());
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);
    if (!initialized) {
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

    if (currentRect) {
        QRectF newRect(startPos, currentPos);
        currentRect->setRect(newRect.normalized());
    }

    emit onMousePosChanged(currentPos);
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && currentRect) {
        // ToDo: Check if rect is too small to add
        if (!rectangleList.contains(currentRect) ) {
            rectangleList.append(currentRect);
        }
        if (rectangleList.contains(currentRect) && currentRect->getVisibleArea() < 60) {
            rectangleList.removeAll(currentRect);
            currentRect->removeFromScene(scene);
        }
        currentRect = nullptr;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageWidget::updateRect() {
    qreal scale = 1 / transform().m11();
    for (int i = 0; i < rectangleList.size(); ++i) {
        rectangleList[i]->setScale(scale);
    }
}

int ImageWidget::activateRectByPoint(QPointF point) {
    QPointF *preActivatedPoint = nullptr;
    qreal distance = 9999;

    for (int i = 0; i < rectangleList.size(); ++i) {
        preActivatedPoint = rectangleList[i]->getCornerPoint(point);
        if (!preActivatedPoint) {
            continue;
        } else if (!currentRect || QLineF(*preActivatedPoint, point).length() < distance) {
            currentRect = rectangleList[i];
            startPos = *currentRect->getOppositePoint(*preActivatedPoint);
            distance = abs(QLineF(startPos, point).length());
        }
    }
    return currentRect ? 0 : -1;
}
