#include "vertical_resize_widget.h"
#include <QPalette>
#include <QGridLayout>

VerticalResizeWidget::VerticalResizeWidget(QWidget *parent) : QWidget(parent) {
    setFixedWidth(4);
    setMinimumHeight(100);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: lightgray; border: 0px solid lightgray;");

    QLayout *mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    lineWidget = new QWidget(this);
    lineWidget->setStyleSheet("background-color: darkgrey; border-radius: 1px; border: 2px solid darkgrey;");
    lineWidget->setFixedWidth(2);
    lineWidget->setMinimumHeight(40);

    mainLayout->addWidget(lineWidget);
    setCursor(Qt::SizeHorCursor);
}

void VerticalResizeWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}
