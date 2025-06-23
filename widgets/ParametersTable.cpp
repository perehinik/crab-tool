#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QLayout>

#include "ParametersTable.h"
#include "PointEditWidget.h"
#include "MultiValueWidget.h"

ParametersTable::ParametersTable(QWidget *parent)
    : QTableWidget(0, 2, parent)
{
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    int rowId;

    rowId = addParameter("Object");
    MultiValueWidget *objectsEdit = new MultiValueWidget(this);
    setCellWidget(rowId, 1, objectsEdit);
    connect(objectsEdit, &MultiValueWidget::valuesChanged, this, [=]() {
        setRowHeight(rowId, objectsEdit->sizeHint().height());
    });

    rowId = addParameter("Top Left");
    PointEditWidget *edit = new PointEditWidget(this);
    setCellWidget(rowId, 1, edit);

    rowId = addParameter("Bot Right");
    PointEditWidget *edit1 = new PointEditWidget(this);
    setCellWidget(rowId, 1, edit1);
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
