#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include "file_manager_widget.h"
#include "image_widget.h"
#include "navigator_widget.h"
#include "toolbox_widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void onImageClicked(QString imagePath);
    ~MainWindow();

private:
    FileManager *fileManagerWidget;
    ImageWidget *imageWidget;
    NavigatorWidget *navigatorWidget;
};
#endif // MAINWINDOW_H
