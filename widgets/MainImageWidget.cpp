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
}

void ImageWidget::setImage(QString imagePath) {
    setResized(false);
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
}

void ImageWidget::checkZoom() {
    if (!scene) {
        return;
    }
    // Prevent zooming out beyond full-scene view
    QRectF viewBounds = mapToScene(viewport()->rect()).boundingRect();
    QRectF sceneBounds = scene->sceneRect();

    if (sceneBounds.width() <= viewBounds.width() * 3) {
        // Use better transformation for zoomed out to get better image
        imageItem->setTransformationMode(Qt::SmoothTransformation);
    } else {
        // Use fast for zoomed image so pixels are visible
        imageItem->setTransformationMode(Qt::FastTransformation);
    }

    if (viewBounds.contains(sceneBounds)) {
        setResized(false);
    }
}

void ImageWidget::setResized(bool isResized) {
    if (isResized) {
        this->resized = true;
    } else {
        this->resized = false;
        zoomLevel = 1.0;
        fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void ImageWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    if (!initialized) {
        return;
    }
    if (scene && scene->sceneRect().isValid() && !resized) {
        setResized(false);
    }
    checkZoom();
    updateRect();
}

void ImageWidget::setZoom(double factor, double newZoomLevel) {
    // Clamp zoom level to reasonable range
    if (newZoomLevel > maxZoom) {return;}

    // Apply zoom
    scale(factor, factor);
    setResized(std::round(newZoomLevel * 100) / 100 > 1.0);
    zoomLevel = newZoomLevel;

    // Prevent zooming out beyond full-scene view
    checkZoom();
    // rectanglePen.setWidth(2*zoomLevel);
    updateRect();
}

void ImageWidget::zoomIn() {
    double factor = zoomStep;
    double newZoomLevel = zoomLevel * factor;
    setZoom(factor, newZoomLevel);
}

void ImageWidget::zoomOut() {
    double factor = 1.0 / zoomStep;
    double newZoomLevel = zoomLevel * factor;
    setZoom(factor, newZoomLevel);
}

void ImageWidget::zoomToExtent() {
    double factor = 1.0 / zoomStep;
    double newZoomLevel = 1;
    setZoom(factor, newZoomLevel);
}

void ImageWidget::wheelEvent(QWheelEvent *event) {
    if (!initialized) {
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
    if (currentRect) {
        QPointF currentPos = mapToScene(event->pos());
        QSize imageSize = imageItem->pixmap().size();
        int imageWidth = imageSize.width();
        int imageHeight = imageSize.height();

        if (currentPos.x() < 0) { currentPos.setX(0); }
        if (currentPos.y() < 0) { currentPos.setY(0); }
        if (currentPos.x() > imageWidth) { currentPos.setX(imageWidth); }
        if (currentPos.y() > imageHeight) { currentPos.setY(imageHeight); }

        QRectF newRect(startPos, currentPos);
        currentRect->setRect(newRect.normalized());
    }
    QGraphicsView::mouseMoveEvent(event);
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
