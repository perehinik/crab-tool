#include <QDockWidget>
#include <QJsonObject>
#include <QFileDialog>

#include "MainWindow.h"
#include "Constants.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    rootJson = QJsonObject();
    toolbox = new ToolboxWidget(this);
    connect(toolbox->handToolButton, &QToolButton::toggled, this, &MainWindow::onMoveChanged);

    connect(toolbox, &ToolboxWidget::onDirOpen, this, &MainWindow::onDirOpen);
    connect(toolbox, &ToolboxWidget::onImagesOpen, this, &MainWindow::onFilesOpen);
    connect(toolbox, &ToolboxWidget::onProjectOpen, this, &MainWindow::openProject);
    connect(toolbox->saveButton->action, &QAction::triggered, this, &MainWindow::onSaveProjectClick);

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

    onDirOpen("/home/ivan/proj/TrainingData/");
}

void MainWindow::onSaveProjectClick() {
    QString projectFilePath = QDir(projectDir).filePath(projectFile);
    if (projectFile == TEMP_PROJECT_FILENAME) {
        QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save Project File",
            projectDir,
            "Project (*.ctp)"
            );

        if (fileName.isEmpty()) {
            return;
        }
        if (!fileName.endsWith(PROJECT_EXTENSION)) {
            fileName+=".";
            fileName+=PROJECT_EXTENSION;
        }
        projectFilePath = fileName;
    }
    qDebug() << "Save" << projectFilePath;
    saveProject(projectFilePath);
}

void MainWindow::saveProject(QString projectPath) {
    // Save latest changes to root object
    if (!imageWidget->imagePath.isEmpty()) {
        rootJson[imageWidget->imagePath] = imageWidget->toJson();
    }

    projectDir = QFileInfo(projectPath).absolutePath();
    projectFile = QFileInfo(projectPath).fileName();
    toolbox->setCurrentDir(projectDir);
    QFile file(projectPath);
    QJsonDocument doc(rootJson);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // or Compact
        file.close();
    } else {
        qWarning() << "Could not open file for writing:" << file.errorString();
    }
}

void MainWindow::openProject(QString projectPath) {
    qDebug() << "Open project" << projectPath;
    QFile file(projectPath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file:" << file.errorString();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (doc.isNull()) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "JSON is not an object";
        return;
    }

    rootJson = doc.object();
    projectDir = QFileInfo(projectPath).absolutePath();
    projectFile = QFileInfo(projectPath).fileName();
    toolbox->setCurrentDir(projectDir);
    dirNavigatorWidget->setPath(projectDir);
    dirNavigatorDock->show();
}

void MainWindow::onDirOpen(QString dirPath) {
    if (QDir(projectDir).exists() && !projectFile.isEmpty()) {
        saveProject(QDir(projectDir).filePath(projectFile));
        rootJson = QJsonObject();
    }
    if (!QDir(dirPath).exists()) {
        return;
    }
    projectDir = dirPath;
    toolbox->setCurrentDir(projectDir);
    projectFile = TEMP_PROJECT_FILENAME;
    QString projectPath = QDir(projectDir).filePath(projectFile);
    if (!QFile(projectPath).exists()) {
        saveProject(projectPath);
    } else {
        openProject(projectPath);
    }
    dirNavigatorWidget->setPath(dirPath);
    dirNavigatorDock->show();
}

void MainWindow::onMoveChanged() {
    if (toolbox->handToolButton->isChecked()) {
        imageWidget->setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
        imageWidget->setDragMode(QGraphicsView::NoDrag);
    }
}

void MainWindow::onFilesOpen(QStringList filePathList) {
    if (filePathList.length() == 0) {
        return;
    }
    if (QDir(projectDir).exists() && !projectFile.isEmpty()) {
        saveProject(QDir(projectDir).filePath(projectFile));
    }
    QString newProjectDir = QFileInfo(filePathList[0]).absolutePath();
    toolbox->setCurrentDir(newProjectDir);
    if (newProjectDir != projectDir) {
        rootJson = QJsonObject();
        projectDir = newProjectDir;
        projectFile = TEMP_PROJECT_FILENAME;
        QString projectPath = QDir(projectDir).filePath(projectFile);
        if (!QFile(projectPath).exists()) {
            saveProject(projectPath);
        } else {
            openProject(projectPath);
        }
    }
    imageNavigatorWidget->loadItems(filePathList);
    dirNavigatorDock->hide();
}

void MainWindow::onPathChanged(QString dirPath) {
    imageNavigatorWidget->setPath(dirPath);
}

void MainWindow::onImageClicked(QString imagePath)
{
    rootJson[imageWidget->imagePath] = imageWidget->toJson();
    imageWidget->setImage(imagePath);
    imageZoomWidget->setImage(imagePath);
    if (rootJson.contains(imagePath)) {
        imageWidget->fromJson(rootJson[imagePath].toObject());
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    dirNavigatorWidget->setMaximumHeight(10000);
}

MainWindow::~MainWindow() {}
