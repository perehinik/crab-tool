#ifndef TOOLBOX_WIDGET_H
#define TOOLBOX_WIDGET_H

#include "ToolboxButton.h"
#include <QWidget>

class ToolboxWidget : public QWidget {
    Q_OBJECT

public:
    explicit ToolboxWidget(QWidget *parent = nullptr);
    void onOpenFile();
    void onOpenDir();

    ToolboxButton *saveButton;
    ToolboxButton *handToolButton;
    ToolboxButton *zoomInToolButton;
    ToolboxButton *zoomOutToolButton;
    ToolboxButton *zoomToExtentsToolButton;

signals:
    void onDirOpen(QString dirPath);
    void onFilesOpen(QStringList filePathList);
};

#endif // TOOLBOX_WIDGET_H
