#include "image_button_widget.h"
#include <QFileInfo>
#include <QImageReader>
#include <QEvent>

ImgButtonWidget::ImgButtonWidget(QString imagePath, QWidget *parent) : QWidget(parent) {
    this->imagePath = imagePath;
    QFileInfo fileInfo(imagePath);
    QString fileName = fileInfo.fileName();
    nameLabel = new QLabel(fileName, this);

    QImageReader reader(imagePath);
    QSizeF size = reader.size();
    imageScale = size.height() / size.width();

    // Load the image
    pixmap = QPixmap(imagePath);

    qreal maxSize = std::max(pixmap.height(), pixmap.width());
    qreal scalingFactor = maxSize / 250;
    pixmap = pixmap.scaled(QSize(pixmap.width() / scalingFactor, pixmap.height() / scalingFactor),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(pixmap.scaled(QSize(100, 100*imageScale), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    backgroundLabel->setAlignment(Qt::AlignCenter);

    updateBackground();

    QGridLayout *layout_w = new QGridLayout(this);
    layout_w->addWidget(backgroundLabel, 0, 0);
    layout_w->addWidget(nameLabel, 1, 0);
    layout_w->setContentsMargins(0, 0, 0, 0);
    layout_w->setSpacing(0);
    layout_w->setAlignment(Qt::AlignCenter);
    nameLabel->setAlignment(Qt::AlignCenter);
    setLayout(layout_w);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void ImgButtonWidget::updateBackground() {
    qreal parentWidth = parentWidget() ? parentWidget()->width() : width();

    qreal newWidth = parentWidth * 0.8;
    qreal newHeight = newWidth * imageScale;

    if (!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap.scaled(QSize(newWidth, newHeight),
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation));
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
