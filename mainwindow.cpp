#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *layout_main = new QGridLayout();
    layout_main->setContentsMargins(0, 0, 0, 0);
    layout_main->setSpacing(0);

    QWidget *toolbox = new ToolboxWidget();
    QWidget *secondaryWidget = new QWidget(this);

    // Add widgets to layout with stretch factors (relative widths)
    layout_main->addWidget(toolbox, 0, 0);
    layout_main->addWidget(secondaryWidget, 1, 0);

    centralWidget->setMinimumSize(800, 300);
    centralWidget->setLayout(layout_main);

    QHBoxLayout *layout_secondary = new QHBoxLayout();
    layout_secondary->setContentsMargins(0, 0, 0, 0);
    layout_secondary->setSpacing(0);

    // Create three widgets (buttons as example)
    fileManagerWidget = new FileManager(secondaryWidget);
    imageWidget = new ImageWidget(secondaryWidget, "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg");
    navigatorWidget = new NavigatorWidget(secondaryWidget);

    QObject::connect(fileManagerWidget, &FileManager::onImageButtonClicked, this, &MainWindow::onImageClicked);

    fileManagerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Add widgets to layout with stretch factors (relative widths)
    layout_secondary->addWidget(fileManagerWidget, 1); // 1 part width
    layout_secondary->addWidget(imageWidget, 5); // 2 part width
    layout_secondary->addWidget(navigatorWidget, 2); // 2 part width

    // Set layout to central widget
    secondaryWidget->setLayout(layout_secondary);
    setCentralWidget(centralWidget);
}

void MainWindow::onImageClicked(QString imagePath)
{
    imageWidget->setImage(imagePath);
}

MainWindow::~MainWindow() {}
