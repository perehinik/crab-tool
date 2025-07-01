#ifndef TOOLBOXWIDGET_H
#define TOOLBOXWIDGET_H

#include <QWidget>
#include <QDir>

#include "ToolboxButtonWidget.h"

class ToolboxWidget : public QWidget {
    Q_OBJECT

public:
    explicit ToolboxWidget(QWidget *parent = nullptr);
    void onOpenProject();
    void onOpenImages();
    void onOpenDir();
    void setCurrentDir(QString dirPath);

    ToolboxButton *saveButton;
    ToolboxButton *handToolButton;
    ToolboxButton *zoomInToolButton;
    ToolboxButton *zoomOutToolButton;
    ToolboxButton *zoomToExtentsToolButton;

private:
    QDir currentDir;

signals:
    void onDirOpen(QString dirPath);
    void onProjectOpen(QString filePath);
    void onImagesOpen(QStringList filePathList);
};

#endif // TOOLBOXWIDGET_H
