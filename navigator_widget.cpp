#include "navigator_widget.h"
#include <QPalette>

NavigatorWidget::NavigatorWidget(QWidget *parent) : QWidget(parent) {
    // setFixedWidth(300);

    // Set gray background using palette
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::green);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}
