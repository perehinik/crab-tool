#include <QGridLayout>

#include "SelectionPopup.h"

SelectionPopup::SelectionPopup(QWidget *parent) : QWidget(parent) {
    deleteBtn = new QPushButton("Delete");
    deleteBtn->setIcon(QIcon(":/icon/delete.png"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(deleteBtn);
    connect(deleteBtn, &QPushButton::clicked, this, [this]() { emit onDeleteClicked(); });
}
