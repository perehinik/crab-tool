#include "image_button_widget.h"
#include <QFileInfo>
#include <QEvent>

ImgButtonWidget::ImgButtonWidget(QString imagePath, QWidget *parent) : QWidget(parent) {
    this->imagePath = imagePath;
    QFileInfo fileInfo(imagePath);
    QString fileName = fileInfo.fileName();
    nameLabel = new QLabel(fileName, this);

    // Load the image
    pixmap = QPixmap(imagePath);

    qreal maxSize = std::max(pixmap.height(), pixmap.width());
    qreal scalingFactor = maxSize / 250;
    pixmap = pixmap.scaled(QSize(pixmap.width() / scalingFactor, pixmap.height() / scalingFactor),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(pixmap.scaled(QSize(100, 100), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    backgroundLabel->setAlignment(Qt::AlignCenter);
    // backgroundLabel->lower();

    updateBackground();

    // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // setFixedHeight(200);

    QVBoxLayout *layout_w = new QVBoxLayout(this);
    layout_w->addWidget(backgroundLabel);
    layout_w->addWidget(nameLabel);
    layout_w->setContentsMargins(0, 0, 0, 0);
    layout_w->setSpacing(0);
    layout_w->setAlignment(Qt::AlignCenter);
    nameLabel->setAlignment(Qt::AlignCenter);
    setLayout(layout_w);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // backgroundLabel->setFixedHeight(100);
    // backgroundLabel->setFixedWidth(150);
}

void ImgButtonWidget::updateBackground() {
    int parentWidth = parentWidget() ? parentWidget()->width() : width();
    int parentHeight = parentWidget() ? parentWidget()->height() : height();

    // if (parentWidth % 10 == previousWidth) {
    //     return;
    // }
    // previousWidth = parentWidth % 10;

    if (!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap.scaled(QSize(parentWidth * 0.7, parentHeight * 0.7),
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation));
    }
}

void ImgButtonWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    setFixedWidth(parentWidget()->width() * 0.8);

    updateBackground();
}

void ImgButtonWidget::mousePressEvent(QMouseEvent *event)
{
    emit onImageButtonClicked(imagePath);
}
