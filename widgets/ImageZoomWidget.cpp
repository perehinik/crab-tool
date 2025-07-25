#include <QPalette>
#include <QTimer>

#include "ImageZoomWidget.h"

ImageZoomWidget::ImageZoomWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("border: solid lightgrey; border-width: 0px;");
    setMinimumWidth(300);

    layout = new QGridLayout(this);
    setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);
    layout->setHorizontalSpacing(5);

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(view);
    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setDragMode(QGraphicsView::NoDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *spacingLeft = new QWidget(this);
    QWidget *spacingRight = new QWidget(this);
    spacingLeft->setFixedWidth(10);
    spacingRight->setFixedWidth(10);

    zoomOutButton = new ToolboxButton(QIcon(":/icon/landscape.png"), "Zoom Out", 22, 15, this);
    zoomInButton = new ToolboxButton(QIcon(":/icon/landscape.png"), "Zoom In", 22, 20, this);

    connect(zoomOutButton->action, &QAction::triggered, this, &ImageZoomWidget::zoomOut);
    connect(zoomInButton->action, &QAction::triggered, this, &ImageZoomWidget::zoomIn);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(1, 100);
    slider->setValue(1);
    slider->setStyleSheet(R"(
    QSlider::groove:horizontal {
        background: #cccccc;       /* flat groove color */
        height: 4px;
        border-radius: 3px;
    }

    QSlider::sub-page:horizontal {
        background: transparent;   /* disable the filled part */
    }

    QSlider::add-page:horizontal {
        background: transparent;   /* disable unfilled part */
    }

    QSlider::handle:horizontal {
        background: #666666;
        width: 6px;
        height: 14px;
        margin: -4px 0;
        border-radius: 3px;
    }
    )");

    connect(slider, &QSlider::valueChanged, this, &ImageZoomWidget::onSliderValueChanged);

    layout->addWidget(view, 0, 0, 1, 5);
    layout->addWidget(spacingLeft, 1, 0, 1, 1, Qt::AlignVCenter);
    layout->addWidget(zoomOutButton, 1, 1, 1, 1, Qt::AlignVCenter);
    layout->addWidget(slider, 1, 2, 1, 1, Qt::AlignVCenter);
    layout->addWidget(zoomInButton, 1, 3, 1, 1, Qt::AlignVCenter);
    layout->addWidget(spacingRight, 1, 4, 1, 1, Qt::AlignVCenter);

    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 0);

    setImage(imagePath);
}

QSize ImageZoomWidget::sizeHint() const {
    return QSize(width(), view->height() + zoomInButton->height());
}

void ImageZoomWidget::setImage(QString imagePath) {
    initialized = false;
    pixmap = QPixmap(imagePath);
    if (!pixmap) {
        pixmap = QPixmap(300, 150);
        pixmap.fill(QColor(204, 204, 204));
    }
    if (!pixmap.isNull()) {
        if (imageItem) {
            imageItem->setPixmap(pixmap);
        } else {
            imageItem = scene->addPixmap(pixmap);
        }
        imageItem->setPos(0, 0);
        scene->setSceneRect(pixmap.rect());
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        setZoom(1);

        if (positionTarget) {
            positionTarget->updateSceneSize(pixmap.size());
        } else {
            positionTarget = new PositionTarget(scene, pixmap.size(), QPointF(0, 0), 6);
        }
        QPointF targetPos = QPointF(static_cast<qreal>(scene->width()) / 2, static_cast<qreal>(scene->height()) / 2);
        updatePositionTarget(targetPos);

        QTimer::singleShot(10, this, [this]() {
            this->resizeEvent(nullptr);
        });
        initialized = true;
    }
    repaint();
}

void ImageZoomWidget::updatePositionTarget(QPointF pos, qreal targetScale) {
    if (positionTarget && scene && !pixmap.isNull() && pixmap.width() > 0 && width() > 0) {
        if (std::round(targetScale * 1000) <= 0) {
            targetScale = 1.0 / (static_cast<qreal>(width()) / static_cast<qreal>(pixmap.width()));
        }

        positionTarget->setScale(targetScale);
        if (!pos.isNull()) {
            positionTarget->setPosition(pos);
        }
    }
}

void ImageZoomWidget::setZoom(double newZoomLevel) {
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    zoomLevel = newZoomLevel;

    if (std::round(zoomLevel * 1000) == 1000) {
        QPointF targetPos = QPointF(static_cast<qreal>(pixmap.width()) / 2, static_cast<qreal>(pixmap.height()) / 2);
        updatePositionTarget(targetPos);
        return;
    }
    qreal trans = view->transform().m11();
    qreal scaleMax = MIN_PIXEL_DENSITY / trans;
    // Calculate new scale using logarithmic interpolation
    qreal newScale = std::pow(scaleMax, (newZoomLevel - slider->minimum()) / (slider->maximum() - slider->minimum()));

    view->scale(newScale, newScale);
    QPointF targetPos = QPointF(static_cast<qreal>(scene->width()) / 2, static_cast<qreal>(scene->height()) / 2);
    updatePositionTarget(targetPos, 1.0 / (trans * newScale));
}

void ImageZoomWidget::centerOn(QPointF position) {
    view->centerOn(position);
    if (positionTarget && !pixmap.isNull()) {
        positionTarget->setPosition(position);
    }
}

void ImageZoomWidget::zoomIn() {
    slider->setValue(zoomLevel + 1);
}

void ImageZoomWidget::zoomOut() {
    slider->setValue(zoomLevel - 1);
}

void ImageZoomWidget::zoomToExtent() {
    slider->setValue(1);
}

void ImageZoomWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if (pixmap.isNull())
        return;

    qreal aspect = static_cast<qreal>(pixmap.width()) / pixmap.height();
    int viewWidth = this->width();
    int viewHeight = static_cast<int>(viewWidth / aspect);

    view->setFixedHeight(viewHeight);
    if (std::round(zoomLevel * 1000) == 1000) {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
    QPointF targetPos = QPointF(static_cast<qreal>(scene->width()) / 2, static_cast<qreal>(scene->height()) / 2);
    updatePositionTarget(targetPos, 1.0 / view->transform().m11());
    this->setZoom(slider->value());
}

void ImageZoomWidget::onSliderValueChanged(int value) {
    setZoom(static_cast<qreal>(value));
}

ImageZoomWidget::~ImageZoomWidget() {
    delete positionTarget;
}
