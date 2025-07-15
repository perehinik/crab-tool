#ifndef INFOPOPUP_H
#define INFOPOPUP_H

#include <QDialog>

class QLabel;

class InfoPopup : public QDialog {
    Q_OBJECT

public:
    explicit InfoPopup(QWidget* parent = nullptr);
};

#endif // INFOPOPUP_H
