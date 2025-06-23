#ifndef POINTEDITWIDGET_H
#define POINTEDITWIDGET_H

#include <QWidget>
#include <QSpinBox>

class PointEditWidget : public QWidget {
    Q_OBJECT

public:
    explicit PointEditWidget(QWidget *parent = nullptr);

    QPoint point();
    void setPoint(QPoint point);

private:
    QSpinBox *editX = nullptr;
    QSpinBox *editY = nullptr;

signals:
    void onPointChanged(QPoint point);
};

#endif // POINTEDITWIDGET_H
