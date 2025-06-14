#ifndef TOOLBOXWIDGET_H
#define TOOLBOXWIDGET_H

#include <QWidget>

#include "ToolboxButtonWidget.h"

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

#endif // TOOLBOXWIDGET_H
