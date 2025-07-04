#include <QDockWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    projectData = new ProjectData();
    projectUpdated = false;
    toolbox = new ToolboxWidget(this);
    connect(toolbox->handToolButton, &QToolButton::toggled, this, &MainWindow::onMouseToolChanged);

    connect(toolbox->openDirButton->action, &QAction::triggered, this, &MainWindow::openDirClickHandler);
    connect(toolbox->openImagesButton->action, &QAction::triggered, this, &MainWindow::openImagesClickHandler);
    connect(toolbox->openProjectButton->action, &QAction::triggered, this, &MainWindow::openProjectClickHandler);
    connect(toolbox->createProjectButton->action, &QAction::triggered, this, &MainWindow::createProjectClickHandler);
    connect(toolbox->saveButton->action, &QAction::triggered, this, &MainWindow::saveProjectClickHandler);

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
    imageZoomWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    imageZoomDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QObject::connect(imageWidget, &ImageWidget::onMousePosChanged, imageZoomWidget, &ImageZoomWidget::centerOn);

    parametersTableWidget = new ParametersTable(this);
    parametersTableWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    parametersTableDock = new QDockWidget("Parameters", this);
    parametersTableDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    parametersTableDock->setWidget(parametersTableWidget);
    parametersTableDock->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    QObject::connect(imageWidget, &ImageWidget::onSelectionChanged, parametersTableWidget, &ParametersTable::setSelection);

    setMenuWidget(toolbox);
    setCentralWidget(imageWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dirNavigatorDock);
    addDockWidget(Qt::LeftDockWidgetArea, imageNavigatorDock);
    addDockWidget(Qt::RightDockWidgetArea, imageZoomDock);
    addDockWidget(Qt::RightDockWidgetArea, parametersTableDock);

    projectData->openDir("/home/ivan/proj/TrainingData/RAW");
    dirNavigatorWidget->setPath(projectData->projectDir());
    imageNavigatorWidget->setPath(projectData->projectDir());
    dirNavigatorDock->show();
    imageWidget->clear();
}

void MainWindow::saveProjectClickHandler() {
    saveSelectionsToProject();
    projectData->saveWithDialog();
}

void MainWindow::createProjectClickHandler() {
    saveSelectionsToProject();
    if (projectData->createWithDialog() == 0) {
        dirNavigatorWidget->setPath(projectData->projectDir());
        imageNavigatorWidget->setPath(projectData->projectDir());
        dirNavigatorDock->show();
        imageWidget->clear();
    }
}

void MainWindow::openProjectClickHandler() {
    saveSelectionsToProject();
    if (projectData->openWithDialog() == 0) {
        dirNavigatorWidget->setPath(projectData->projectDir());
        imageNavigatorWidget->setPath(projectData->projectDir());
        dirNavigatorDock->show();
        imageWidget->clear();
    }

    // Update this at some point, allValues should be private and have different name.
    parametersTableWidget->objectsEdit->allValues = projectData->allTagsCount();
}

void MainWindow::openDirClickHandler() {
    saveSelectionsToProject();
    if (projectData->openDirWithDialog() == 0) {
        dirNavigatorWidget->setPath(projectData->projectDir());
        imageNavigatorWidget->setPath(projectData->projectDir());
        dirNavigatorDock->show();
        imageWidget->clear();
    }
}

void MainWindow::openImagesClickHandler() {
    saveSelectionsToProject();
    QStringList fileList = projectData->openImagesWithDialog();

    if (!fileList.isEmpty()) {
        imageNavigatorWidget->loadItems(fileList);
        imageWidget->clear();
        dirNavigatorDock->hide();
    }
}

void MainWindow::onMouseToolChanged() {
    if (toolbox->handToolButton->isChecked()) {
        imageWidget->setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
        imageWidget->setDragMode(QGraphicsView::NoDrag);
    }
}

void MainWindow::onPathChanged(QString dirPath) {
    imageNavigatorWidget->setPath(dirPath);
}

void MainWindow::onImageClicked(QString imagePath) {
    if (!QFile(imagePath).exists()) {
        return;
    }
    saveSelectionsToProject();
    imageWidget->setImage(imagePath);
    imageZoomWidget->setImage(imagePath);

    QJsonObject data = projectData->getImageData(imageWidget->hash);
    if (!data.isEmpty()) {
        imageWidget->fromJson(data);
    }
}

void MainWindow::saveSelectionsToProject() {
    if (imageWidget->isInitialized()) {
        projectData->updateImageData(imageWidget->hash, imageWidget->toJson(), imageWidget->selectionCount());
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    dirNavigatorWidget->setMaximumHeight(10000);
}

MainWindow::~MainWindow() {
    delete projectData;
}
