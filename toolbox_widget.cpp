#include "toolbox_widget.h"
#include <QPalette>

ToolboxWidget::ToolboxWidget(QWidget *parent) : QWidget(parent) {
    setFixedHeight(30);

    // Set gray background using palette
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::blue);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}
