#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
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
#include "ProjectData.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void onPathChanged(QString dirPath);
    void onImageClicked(QString imagePath);
    ~MainWindow();

private:
    ProjectData *projectData;
    bool projectUpdated;
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
    QLabel *projectPathLabel;

    void showEvent(QShowEvent *event) override;
    void onDirOpen(QString dirPath);
    void onFilesOpen(QStringList filePathList);
    void onMouseToolChanged();
    void saveProject(QString projectPath);
    void openProject(QString projectPath);
    void saveProjectClickHandler();
    void openProjectClickHandler();
    void createProjectClickHandler();
    void openDirClickHandler();
    void openImagesClickHandler();
    void updateProjectPathLabel();
    void updateProjectFile(QString projDir, QString projFile);
    void projectStatusUpdateHandler(ProjectStatus status);
    void exportProject(const QString type);
};
#endif // MAINWINDOW_H
