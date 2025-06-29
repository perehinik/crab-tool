#include <QGridLayout>
#include <QLabel>
#include <QIntValidator>

#include "PointEditWidget.h"

PointEditWidget::PointEditWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(2);
    layout->setContentsMargins(5, 0, 5, 0);

    editX = new QSpinBox(this);
    editY = new QSpinBox(this);
    editX->setMaximumWidth(120);
    editY->setMaximumWidth(120);
    editX->setMinimumWidth(30);
    editY->setMinimumWidth(30);
    editX->setMaximum(999999);
    editY->setMaximum(999999);
    editX->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    editY->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(new QLabel("X:", this), 0, 0);
    layout->addWidget(editX, 0, 1);
    layout->addWidget(new QLabel("Y:", this), 0, 2);
    layout->addWidget(editY, 0, 3);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(3, 1);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer, 0, 4);
}

QPoint PointEditWidget::point() {
    return QPoint(editX->value(), editY->value());
}

void PointEditWidget::setPoint(QPoint point) {
    editX->setValue(point.x());
    editY->setValue(point.y());
}
