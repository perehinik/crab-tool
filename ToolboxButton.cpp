#include "ToolboxButton.h"

ToolboxButton::ToolboxButton(const QIcon &icon, const char *name, int size, int iconSize, QWidget *parent)
    : QToolButton(parent)
{
    action = new QAction(icon, name, this);

    setDefaultAction(action);
    setFixedSize(size, size);
    setIconSize(QSize(iconSize, iconSize));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    setStyleSheet(R"(
        QToolButton {
            border: 0px solid grey;
            margin: 0px;
            padding: 0px;
            background: transparent;
        }

        QToolButton:hover {
            background-color: #e0f0ff;
        }

        QToolButton:pressed {
            background-color: #c0e0ff;
        }
    )");
}
