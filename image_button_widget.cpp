#include "image_button_widget.h"
#include <QFileInfo>
#include <QImageReader>
#include <QEvent>
#include <QThreadPool>

ImgButtonWidget::ImgButtonWidget(QString imagePath, QWidget *parent) : QWidget(parent) {
    this->imagePath = imagePath;
    QFileInfo fileInfo(imagePath);
    nameLabel = new QLabel(fileInfo.fileName(), this);

    backgroundLabel = new QLabel(this);
    backgroundLabel->setAlignment(Qt::AlignCenter);
    backgroundLabel->setFixedSize(100, 100);

    QGridLayout *layout_w = new QGridLayout(this);
    layout_w->addWidget(backgroundLabel, 0, 0);
    layout_w->addWidget(nameLabel, 1, 0);
    layout_w->setContentsMargins(0, 0, 0, 0);
    layout_w->setSpacing(0);
    layout_w->setAlignment(Qt::AlignCenter);
    nameLabel->setAlignment(Qt::AlignCenter);
    setLayout(layout_w);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    loadImage();
}

void ImgButtonWidget::loadImage() {
    QThreadPool::globalInstance()->start([=]() {
        // First load scaled image with fast transformation
        // Resolution should be a bit better than one that will be finally used
        QImageReader reader(imagePath);
        QSizeF size = reader.size();
        imageScale = size.height() / size.width();

        qreal maxOrigSize = std::max(size.height(), size.width());
        qreal resizeOrigFactor = maxOrigSize / (PIXMAP_MAX_SIDE_SIZE * 2);
        if (resizeOrigFactor < 1) { resizeOrigFactor = 1; }

        reader.setScaledSize(QSize(size.width() / resizeOrigFactor, size.height() / resizeOrigFactor));  // Load directly as 64×64 image
        QImage lowResImage = reader.read();
        QPixmap pixmap = QPixmap::fromImage(lowResImage);
        if (!pixmap) {
            QMetaObject::invokeMethod(this, [=]() {
                this->nameLabel->setText("");
                this->setFixedSize(0, 0);
            }, Qt::QueuedConnection);
        }

        // Rescale image with smooth transform
        qreal maxSize = std::max(pixmap.height(), pixmap.width());
        qreal resizeFactor = maxSize / PIXMAP_MAX_SIDE_SIZE;
        pixmap = pixmap.scaled(QSize(pixmap.width() / resizeFactor, pixmap.height() / resizeFactor),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QMetaObject::invokeMethod(this, [=]() {
                this->pixmap = pixmap;
                this->updateBackground();
            }, Qt::QueuedConnection);
    });
}

void ImgButtonWidget::updateBackground() {
    if (!pixmap) {
        return;
    }
    qreal parentWidth = parentWidget() ? parentWidget()->width() : width();
    if (parentWidth > 350) {
        parentWidth = 350;
    }
    if (abs(previousWidth - parentWidth) < 10) {
        return;
    }
    previousWidth = parentWidth;

    qreal newWidth = parentWidth * 0.8;
    qreal newHeight = newWidth * imageScale;

    if (!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap.scaled(QSize(newWidth, newHeight),
                                                 Qt::KeepAspectRatio,
                                                 Qt::FastTransformation));
    }
    backgroundLabel->setFixedSize(QSize(newWidth, newHeight));
}

void ImgButtonWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateBackground();
}

void ImgButtonWidget::mousePressEvent(QMouseEvent *event)
{
    emit onImageButtonClicked(imagePath);
}
