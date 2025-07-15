#ifndef TOOLBOXWIDGET_H
#define TOOLBOXWIDGET_H

#include <QWidget>
#include <QDir>

#include "ToolboxButtonWidget.h"

class QMenuBar;

class ToolboxWidget : public QWidget {
    Q_OBJECT

public:
    explicit ToolboxWidget(QWidget *parent = nullptr);
    QMenuBar *menuBar;

    void onOpenProject();
    void onOpenImages();
    void onOpenDir();

    ToolboxButton *saveButton;
    ToolboxButton *openProjectButton;
    ToolboxButton *createProjectButton;
    ToolboxButton *openImagesButton;
    ToolboxButton *openDirButton;
    ToolboxButton *handToolButton;
    ToolboxButton *zoomInToolButton;
    ToolboxButton *zoomOutToolButton;
    ToolboxButton *zoomToExtentsToolButton;

    void showInfoPopup();

signals:
    void onExportProject(const QString type);
};

#endif // TOOLBOXWIDGET_H
