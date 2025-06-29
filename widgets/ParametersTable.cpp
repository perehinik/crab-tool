#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QLayout>

#include "ParametersTable.h"

ParametersTable::ParametersTable(QWidget *parent)
    : QTableWidget(0, 2, parent)
{
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    int rowId = addParameter("Object");
    objectsEdit = new MultiValueWidget(this);
    setCellWidget(rowId, 1, objectsEdit);
    connect(objectsEdit, &MultiValueWidget::valuesChanged, this, [=]() {
        setRowHeight(rowId, objectsEdit->sizeHint().height());
    });

    rowId = addParameter("Top Left");
    editTopLeft = new PointEditWidget(this);
    setCellWidget(rowId, 1, editTopLeft);

    rowId = addParameter("Bot Right");
    editBotRight = new PointEditWidget(this);
    setCellWidget(rowId, 1, editBotRight);
    setEnabled(false);
}

void ParametersTable::setSelection(SelectionRect * selection) {
    if (currentSelection && !objectsEdit->values().isEmpty()) {
        currentSelection->tags = objectsEdit->values();
    }

    clear();
    currentSelection = selection;
    if (!selection) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
    if (!selection->tags.isEmpty()) {
        objectsEdit->setValues(selection->tags);
    }
    editTopLeft->setPoint(selection->getRect().topLeft().toPoint());
    editBotRight->setPoint(selection->getRect().bottomRight().toPoint());
}

QTableWidgetItem * ParametersTable::createLabel(QString label) {
    QTableWidgetItem *labelItem = new QTableWidgetItem(label);
    labelItem->setFlags(labelItem->flags() & ~Qt::ItemIsEditable);  // Make read-only
    labelItem->setBackground(QBrush(QColor(230, 230, 230)));
    return labelItem;
}

int ParametersTable::addParameter(QString label) {
    int rowId = rowCount();
    insertRow(rowId);
    setItem(rowId, 0, createLabel(label));
    return rowId;
}

void ParametersTable::clear() {
    objectsEdit->clear();
    editTopLeft->setPoint(QPoint(0,0));
    editBotRight->setPoint(QPoint(0,0));
}
