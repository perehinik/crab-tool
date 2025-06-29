#ifndef SELECTIONPOPUP_H
#define SELECTIONPOPUP_H

#include <QWidget>
#include <QPushButton>

class SelectionPopup : public QWidget {
    Q_OBJECT
public:
    explicit SelectionPopup(QWidget *parent = nullptr);

private:
    QPushButton *deleteBtn;

signals:
    void onDeleteClicked();
};

#endif // SELECTIONPOPUP_H
