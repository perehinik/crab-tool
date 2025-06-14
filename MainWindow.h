#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include "DirNavigatorWidget.h"
#include "ImageNavigatorWidget.h"
#include "MainImageWidget.h"
#include "ImageZoomWidget.h"
#include "ToolboxWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void onPathChanged(QString dirPath);
    void onImageClicked(QString imagePath);
    ~MainWindow();

private:
    ToolboxWidget *toolbox;
    DirNavigatorWidget *dirNavigatorWidget;
    ImageNavigatorWidget *imageNavigatorWidget;
    QDockWidget *imageNavigatorDock;
    QDockWidget *dirNavigatorDock;
    ImageWidget *imageWidget;
    ImageZoomWidget *navigatorWidget;

    void showEvent(QShowEvent *event);
    void onDirOpen(QString dirPath);
    void onFilesOpen(QStringList filePathList);
    void onMoveChanged();
};
#endif // MAINWINDOW_H
