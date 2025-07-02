#include <QDockWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

#include "MainWindow.h"
#include "Constants.h"
#include "FileDialog.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    rootJson = QJsonObject();
    imagesJson = QJsonObject();
    projectUpdated = false;
    toolbox = new ToolboxWidget(this);
    connect(toolbox->handToolButton, &QToolButton::toggled, this, &MainWindow::onMoveChanged);

    connect(toolbox->openDirButton->action, &QAction::triggered, this, &MainWindow::onOpenDirClick);
    connect(toolbox->openImagesButton->action, &QAction::triggered, this, &MainWindow::onOpenImagesClick);
    connect(toolbox->openProjectButton->action, &QAction::triggered, this, &MainWindow::onOpenProjectClick);
    connect(toolbox->createProjectButton->action, &QAction::triggered, this, &MainWindow::onCreateProjectClick);
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

    projectFile = TEMP_PROJECT_FILENAME;
    onDirOpen("/home/ivan/proj/TrainingData/");
}

void MainWindow::onSaveProjectClick() {
    QString projectFilePath = QDir(projectDir).filePath(projectFile);
    if (projectFile == TEMP_PROJECT_FILENAME) {
        QString fileName = saveProjectDialog(this, projectDir);
        if (fileName.isEmpty()) {
            return;
        }
        projectFilePath = fileName;
    }
    saveProject(projectFilePath);
}

void MainWindow::onCreateProjectClick() {
    saveSelectionsToJson();
    if (projectUpdated) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Confirm",
            "You're trying to create new project\n"
            "But you have unsaved changes, would you like to save them?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );
        if (reply == QMessageBox::Yes) {
            onSaveProjectClick();
            rootJson = QJsonObject();
            imagesJson = QJsonObject();
            projectUpdated = false;
        } else if (reply == QMessageBox::No) {
            rootJson = QJsonObject();
            imagesJson = QJsonObject();
            projectUpdated = false;
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    QString newProjectPath = openProjectDialog(this, projectDir);
    if (newProjectPath.isEmpty()) {
        return;
    }
    QFile file(newProjectPath);
    if (!file.exists()) {
        // User wants to create completely new project
        updateProjectFile(QFileInfo(newProjectPath).absolutePath(), QFileInfo(newProjectPath).fileName());
        // No need to save for now. User will need to save later.
        return;
    }
}

void MainWindow::onOpenProjectClick() {
    saveSelectionsToJson();
    if (projectUpdated) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Confirm",
            "You're trying to open another project\n"
            "But you have unsaved changes, would you like to save them?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );
        if (reply == QMessageBox::Yes) {
            onSaveProjectClick();
            rootJson = QJsonObject();
            imagesJson = QJsonObject();
            projectUpdated = false;
        } else if (reply == QMessageBox::No) {
            rootJson = QJsonObject();
            imagesJson = QJsonObject();
            projectUpdated = false;
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    QString newProjectPath = openProjectDialog(this, projectDir);
    if (newProjectPath.isEmpty()) {
        return;
    }
    openProject(newProjectPath);
}

void MainWindow::updateProjectFile(QString projDir, QString projFile) {
    projectDir = projDir;
    projectFile = projFile;
}

void MainWindow::saveProject(QString projectPath) {
    // Save latest changes to root object
    saveSelectionsToJson();

    QJsonObject obj = parametersTableWidget->objectsEdit->toJson();
    rootJson["tags-frequency"] = obj["value-frequency"];
    rootJson["project-version"] = PROJECT_VERSION;
    rootJson["project-directory"] = projectDir;
    rootJson["images"] = imagesJson;

    updateProjectFile(QFileInfo(projectPath).absolutePath(), QFileInfo(projectPath).fileName());
    QFile file(projectPath);
    QJsonDocument doc(rootJson);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // or Compact
        projectUpdated = false;
        file.close();
    } else {
        qWarning() << "Could not open file for writing:" << file.errorString();
    }
}

void MainWindow::openProject(QString newProjectPath) {
    rootJson = QJsonObject();
    imagesJson = QJsonObject();
    projectUpdated = false;
    QFile file(newProjectPath);
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
    updateProjectFile(QFileInfo(newProjectPath).absolutePath(), QFileInfo(newProjectPath).fileName());

    dirNavigatorWidget->setPath(projectDir);
    dirNavigatorDock->show();
    imageWidget->clear();

    QJsonObject obj;
    if (rootJson.contains("tags-frequency")) {
        obj["value-frequency"] = rootJson["tags-frequency"].toObject();
        parametersTableWidget->objectsEdit->fromJson(obj);
    }
    if (rootJson.contains("images")) {
        imagesJson = rootJson["images"].toObject();
    }
}

void MainWindow::onOpenDirClick() {
    QString dirPath = openDirDialog(this, projectDir);
    if (dirPath.isEmpty() || !QDir(dirPath).exists()) {
        return;
    }
    onDirOpen(dirPath);
}

void MainWindow::onDirOpen(QString dirPath) {
    if (QDir(projectDir).exists() && !projectFile.isEmpty()) {
        saveProject(QDir(projectDir).filePath(projectFile));
        rootJson = QJsonObject();
        imagesJson = QJsonObject();
        projectUpdated = false;
    }
    if (dirPath != projectDir && projectFile == TEMP_PROJECT_FILENAME) {
        rootJson = QJsonObject();
        imagesJson = QJsonObject();
        projectUpdated = false;
        updateProjectFile(dirPath, TEMP_PROJECT_FILENAME);
        QString projectPath = QDir(projectDir).filePath(projectFile);
        if (!QFile(projectPath).exists()) {
            saveProject(projectPath);
        } else {
            openProject(projectPath);
        }
    }
    dirNavigatorWidget->setPath(dirPath);
    imageNavigatorWidget->setPath(dirPath);
    imageWidget->clear();
    dirNavigatorDock->show();
}

void MainWindow::onOpenImagesClick() {
    QStringList filePathList = openImagesDialog(this, projectDir);
    if (filePathList.isEmpty()) {
        return;
    }
    onFilesOpen(filePathList);
}

void MainWindow::onFilesOpen(QStringList filePathList) {
    if (filePathList.length() == 0) {
        return;
    }
    if (QDir(projectDir).exists() && !projectFile.isEmpty()) {
        saveProject(QDir(projectDir).filePath(projectFile));
        rootJson = QJsonObject();
        imagesJson = QJsonObject();
        projectUpdated = false;
    }
    QString newProjectDir = QFileInfo(filePathList[0]).absolutePath();
    if (newProjectDir != projectDir && projectFile == TEMP_PROJECT_FILENAME) {
        rootJson = QJsonObject();
        imagesJson = QJsonObject();
        projectUpdated = false;
        updateProjectFile(newProjectDir, TEMP_PROJECT_FILENAME);
        QString projectPath = QDir(projectDir).filePath(projectFile);
        if (!QFile(projectPath).exists()) {
            saveProject(projectPath);
        } else {
            openProject(projectPath);
        }
    }
    imageNavigatorWidget->loadItems(filePathList);
    imageWidget->clear();
    dirNavigatorDock->hide();
}

void MainWindow::onMoveChanged() {
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
    saveSelectionsToJson();
    imageWidget->setImage(imagePath);
    imageZoomWidget->setImage(imagePath);
    if (imagesJson.contains(imageWidget->hash)) {
        imageWidget->fromJson(imagesJson[imageWidget->hash].toObject());
    }
}

void MainWindow::saveSelectionsToJson() {
    if (imageWidget->selectionCount() == 0 && imagesJson.contains(imageWidget->hash)) {
        imagesJson.remove(imageWidget->hash);
        projectUpdated = true;
        return;
    }
    if (!imageWidget->hash.isEmpty()) {
        QJsonObject obj = imageWidget->toJson();
        if (obj != imagesJson[imageWidget->hash].toObject()) {
            projectUpdated = true;
        }
        QString relativePath = QDir(projectDir).relativeFilePath(imageWidget->imagePath);
        obj["relative-path"] = relativePath;
        imagesJson[imageWidget->hash] = obj;
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    dirNavigatorWidget->setMaximumHeight(10000);
}

MainWindow::~MainWindow() {}
