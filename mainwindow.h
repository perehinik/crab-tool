#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include "DirNavigatorWidget.h"
#include "ImageNavigatorWidget.h"
#include "image_widget.h"
#include "navigator_widget.h"
#include "toolbox_widget.h"

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
    ImageWidget *imageWidget;
    NavigatorWidget *navigatorWidget;

    void showEvent(QShowEvent *event);
};
#endif // MAINWINDOW_H
