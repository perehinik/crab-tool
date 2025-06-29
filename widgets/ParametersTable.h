#ifndef PARAMETERSTABLE_H
#define PARAMETERSTABLE_H

#include <QTableWidget>
#include <QStringList>

#include "SelectionRect.h"
#include "PointEditWidget.h"
#include "MultiValueWidget.h"

class ParametersTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit ParametersTable(QWidget *parent = nullptr);
    QTableWidgetItem * createLabel(QString label);
    int addParameter(QString label);
    void setSelection(SelectionRect * selection);
    void clear();
    void valuesUpdated();

    MultiValueWidget *objectsEdit;
    PointEditWidget *editTopLeft;
    PointEditWidget *editBotRight;

private:
    SelectionRect * currentSelection;
};

#endif // PARAMETERSTABLE_H
