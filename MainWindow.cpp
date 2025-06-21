#include <QDockWidget>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    toolbox = new ToolboxWidget(this);
    connect(toolbox->handToolButton, &QToolButton::toggled, this, &MainWindow::onMoveChanged);

    QObject::connect(toolbox, &ToolboxWidget::onDirOpen, this, &MainWindow::onDirOpen);
    QObject::connect(toolbox, &ToolboxWidget::onFilesOpen, this, &MainWindow::onFilesOpen);

    imageWidget = new ImageWidget(this);

    connect(toolbox->zoomInToolButton->action, &QAction::triggered, imageWidget, &ImageWidget::zoomIn);
    connect(toolbox->zoomOutToolButton->action, &QAction::triggered, imageWidget, &ImageWidget::zoomOut);
    connect(toolbox->zoomToExtentsToolButton->action, &QAction::triggered, imageWidget, &ImageWidget::zoomToExtent);

    dirNavigatorWidget = new DirNavigatorWidget(this);
    dirNavigatorDock = new QDockWidget("Directories", this);
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
    imageNavigatorDock = new QDockWidget("Images", this);
    imageNavigatorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    imageNavigatorDock->setWidget(imageNavigatorWidget);

    QObject::connect(imageNavigatorWidget, &ImageNavigatorWidget::onImageClicked, this, &MainWindow::onImageClicked);

    imageZoomWidget = new ImageZoomWidget(this);
    imageZoomDock = new QDockWidget("Image Zoom", this);
    imageZoomDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    imageZoomDock->setWidget(imageZoomWidget);

    QObject::connect(imageWidget, &ImageWidget::onMousePosChanged, imageZoomWidget, &ImageZoomWidget::centerOn);

    setMenuWidget(toolbox);
    setCentralWidget(imageWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dirNavigatorDock);
    addDockWidget(Qt::LeftDockWidgetArea, imageNavigatorDock);
    addDockWidget(Qt::RightDockWidgetArea, imageZoomDock);
}

void MainWindow::onDirOpen(QString dirPath)
{
    dirNavigatorWidget->setPath(dirPath);
    dirNavigatorDock->show();
}

void MainWindow::onMoveChanged()
{
    if (toolbox->handToolButton->isChecked()) {
        imageWidget->setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
        imageWidget->setDragMode(QGraphicsView::NoDrag);
    }
}

void MainWindow::onFilesOpen(QStringList filePathList) {
    imageNavigatorWidget->loadItems(filePathList);
    dirNavigatorDock->hide();
}

void MainWindow::onPathChanged(QString dirPath)
{
    imageNavigatorWidget->setPath(dirPath);
}

void MainWindow::onImageClicked(QString imagePath)
{
    imageWidget->setImage(imagePath);
    imageZoomWidget->setImage(imagePath);
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);  // Call base class

    dirNavigatorWidget->setMaximumHeight(10000);
}

MainWindow::~MainWindow() {}
