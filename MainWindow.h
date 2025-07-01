#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <Constants.h>

#include "DirNavigatorWidget.h"
#include "ImageNavigatorWidget.h"
#include "MainImageWidget.h"
#include "ImageZoomWidget.h"
#include "ToolboxWidget.h"
#include "ParametersTable.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void onPathChanged(QString dirPath);
    void onImageClicked(QString imagePath);
    ~MainWindow();

private:
    QString projectDir;
    QString projectFile;
    QJsonObject rootJson;
    ToolboxWidget *toolbox;
    DirNavigatorWidget *dirNavigatorWidget;
    ImageNavigatorWidget *imageNavigatorWidget;
    QDockWidget *imageNavigatorDock;
    QDockWidget *dirNavigatorDock;
    QDockWidget *imageZoomDock;
    ImageWidget *imageWidget;
    ImageZoomWidget *imageZoomWidget;
    ParametersTable *parametersTableWidget;
    QDockWidget *parametersTableDock;

    void showEvent(QShowEvent *event) override;
    void onDirOpen(QString dirPath);
    void onFilesOpen(QStringList filePathList);
    void onMoveChanged();
    void saveProject(QString projectPath);
    void openProject(QString projectPath);
    void onSaveProjectClick();
};
#endif // MAINWINDOW_H
