#ifndef PARAMETERSTABLE_H
#define PARAMETERSTABLE_H

#include <QTableWidget>
#include <QStringList>

class ParametersTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit ParametersTable(QWidget *parent = nullptr);
    QTableWidgetItem * createLabel(QString label);
    int addParameter(QString label);
};

#endif // PARAMETERSTABLE_H
