#ifndef TOOLBOX_WIDGET_H
#define TOOLBOX_WIDGET_H

#include <QWidget>

class ToolboxWidget : public QWidget {
    Q_OBJECT

public:
    explicit ToolboxWidget(QWidget *parent = nullptr);
};

#endif // TOOLBOX_WIDGET_H
