#include "ComboBoxOrdered.h"
#include <QLineEdit>

ComboBoxOrdered::ComboBoxOrdered(QWidget *parent)
    : QComboBox(parent)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setCurrentText("");  // Start with empty text

    connect(lineEdit(), &QLineEdit::editingFinished,
            this, &ComboBoxOrdered::handleEditingFinished);
}

void ComboBoxOrdered::handleEditingFinished()
{
    QString text = currentText().trimmed();
    if (!text.isEmpty() && findText(text) == -1) {
        addItem(text);
        setCurrentText(text);  // Select the newly added item
        emit onItemsChanged();
    }
}

void ComboBoxOrdered::removeCurrentValue()
{
    int index = currentIndex();
    if (index >= 0) {
        removeItem(index);
        setCurrentText("");         // Clear the field
        emit onItemsChanged();      // Notify
    }
}
