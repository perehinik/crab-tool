#include "mainwindow.h"
#include "vertical_resize_widget.h"
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *layout_main = new QGridLayout();
    layout_main->setContentsMargins(0, 0, 0, 0);
    layout_main->setSpacing(0);

    QWidget *secondaryWidget = new QWidget(this);

    // Add widgets to layout with stretch factors (relative widths)
    layout_main->addWidget(secondaryWidget, 1, 0);

    centralWidget->setMinimumSize(800, 300);
    centralWidget->setLayout(layout_main);

    QGridLayout *layout_secondary = new QGridLayout();
    layout_secondary->setContentsMargins(0, 0, 0, 0);
    layout_secondary->setSpacing(0);

    // Create three widgets (buttons as example)

    QWidget *toolbox = new ToolboxWidget();

    imageWidget = new ImageWidget(secondaryWidget, "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg");

    fileManagerWidget = new FileManager(secondaryWidget);
    QDockWidget *dock1 = new QDockWidget("FileNavigator", this);
    dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock1->setWidget(fileManagerWidget);

    navigatorWidget = new NavigatorWidget(secondaryWidget);
    QDockWidget *dock2 = new QDockWidget("ImageNavigator", this);
    dock2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock2->setWidget(navigatorWidget);

    QObject::connect(fileManagerWidget, &FileManager::onImageButtonClicked, this, &MainWindow::onImageClicked);

    fileManagerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMenuWidget(toolbox);
    addDockWidget(Qt::LeftDockWidgetArea, dock1);
    layout_secondary->addWidget(imageWidget, 0, 0);
    addDockWidget(Qt::RightDockWidgetArea, dock2);

    // Set layout to central widget
    secondaryWidget->setLayout(layout_secondary);
    setCentralWidget(centralWidget);
}

void MainWindow::onImageClicked(QString imagePath)
{
    imageWidget->setImage(imagePath);
}

MainWindow::~MainWindow() {}
