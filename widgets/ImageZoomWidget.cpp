#include <QPalette>

#include "ImageZoomWidget.h"

ImageZoomWidget::ImageZoomWidget(QWidget *parent) : QWidget(parent) {
    // setFixedWidth(300);

    // Set gray background using palette
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::green);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}
