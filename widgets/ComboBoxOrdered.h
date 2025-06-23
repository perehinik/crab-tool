#ifndef COMBOBOXORDERED_H
#define COMBOBOXORDERED_H

#include <QComboBox>

class ComboBoxOrdered : public QComboBox {
    Q_OBJECT

public:
    explicit ComboBoxOrdered(QWidget *parent = nullptr);
    void removeCurrentValue();

signals:
    void onItemsChanged();  // Emitted when a new item is added

private slots:
    void handleEditingFinished();
};

#endif // COMBOBOXORDERED_H
