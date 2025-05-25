#ifndef VERTICAL_RESIZE_WIDGET_H
#define VERTICAL_RESIZE_WIDGET_H

#include <QWidget>

class VerticalResizeWidget : public QWidget {
    Q_OBJECT

public:
    explicit VerticalResizeWidget(QWidget *parent = nullptr);

private:
    QWidget *lineWidget;
    void resizeEvent(QResizeEvent *event);
};

#endif // VERTICAL_RESIZE_WIDGET_H
