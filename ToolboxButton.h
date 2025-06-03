#ifndef TOOLBOXBUTTON_H
#define TOOLBOXBUTTON_H

#include <QToolButton>
#include <QAction>

class ToolboxButton : public QToolButton {
    Q_OBJECT

public:
    QAction *action;

    ToolboxButton(const QIcon &icon, const char *name, int size, int iconSize, QWidget *parent);
};

#endif // TOOLBOXBUTTON_H
