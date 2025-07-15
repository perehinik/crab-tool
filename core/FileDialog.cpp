#include "FileDialog.h"
#include "Constants.h"

#include <QFileDialog>

QStringList openImagesDialog(QWidget *parent, QString dirPath) {
    QDir qdir(dirPath);
    return QFileDialog::getOpenFileNames(
        parent,
        "Select Image Files",
        qdir.exists() ? qdir.path() : QDir::homePath(),
        "Images (*.png *.jpg *.bmp);;All Files (*.*)"
    );
}

QString openDirDialog(QWidget *parent, QString dirPath) {
    QDir qdir(dirPath);
    return QFileDialog::getExistingDirectory(
        parent,
        "Select Directory",
        qdir.exists() ? qdir.path() : QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
}

QString openProjectDialog(QWidget *parent, QString dirPath) {
    QDir qdir(dirPath);
    QString result = QFileDialog::getOpenFileName(
        parent,
        "Open Project File",
        qdir.exists() ? qdir.path() : QDir::homePath(),
        "Project (*.ctp)"
    );
    if (result.isEmpty()) {
        return result;
    }
    if (!result.endsWith(PROJECT_EXTENSION)) {
        result+=".";
        result+=PROJECT_EXTENSION;
    }
    return result;
}

QString createProjectDialog(QWidget *parent, QString dirPath) {
    QDir qdir(dirPath);
    QString result = QFileDialog::getSaveFileName(
        parent,
        "Create Project File",
        qdir.exists() ? qdir.path() : QDir::homePath(),
        "Project (*.ctp)"
        );
    if (result.isEmpty()) {
        return result;
    }
    if (!result.endsWith(PROJECT_EXTENSION)) {
        result+=".";
        result+=PROJECT_EXTENSION;
    }
    return result;
}

QString saveProjectDialog(QWidget *parent, QString dirPath) {
    QDir qdir(dirPath);
    QString result = QFileDialog::getSaveFileName(
        parent,
        "Save Project File",
        qdir.exists() ? qdir.path() : QDir::homePath(),
        "Project (*.ctp)"
    );
    if (result.isEmpty()) {
        return result;
    }
    if (!result.endsWith(PROJECT_EXTENSION)) {
        result+=".";
        result+=PROJECT_EXTENSION;
    }
    return result;
}
