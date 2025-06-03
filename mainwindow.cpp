#include "mainwindow.h"
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    toolbox = new ToolboxWidget();

    imageWidget = new ImageWidget(this, "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg");

    dirNavigatorWidget = new DirNavigatorWidget(this);
    QDockWidget *dirNavigatorDock = new QDockWidget("Directories", this);
    dirNavigatorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dirNavigatorDock->setWidget(dirNavigatorWidget);
    setStyleSheet(R"(
    QDockWidget {
        background-color: #dadada;
    }
    QDockWidget::title {
        background-color: #dadada;
        padding: 4px;
    }
    )");
    dirNavigatorWidget->setMaximumHeight(200);

    QObject::connect(dirNavigatorWidget, &DirNavigatorWidget::onDirPathChanged, this, &MainWindow::onPathChanged);

    imageNavigatorWidget = new ImageNavigatorWidget(this);
    QDockWidget *imageNavigatorDock = new QDockWidget("Images", this);
    imageNavigatorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    imageNavigatorDock->setWidget(imageNavigatorWidget);

    QObject::connect(imageNavigatorWidget, &ImageNavigatorWidget::onImageClicked, this, &MainWindow::onImageClicked);

    setMenuWidget(toolbox);
    setCentralWidget(imageWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dirNavigatorDock);
    addDockWidget(Qt::LeftDockWidgetArea, imageNavigatorDock);
}

void MainWindow::onPathChanged(QString dirPath)
{
    imageNavigatorWidget->setPath(dirPath);
}

void MainWindow::onImageClicked(QString imagePath)
{
    imageWidget->setImage(imagePath);
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);  // Call base class

    dirNavigatorWidget->setMaximumHeight(10000);
}

MainWindow::~MainWindow() {}
