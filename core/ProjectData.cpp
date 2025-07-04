#include "ProjectData.h"
#include "Constants.h"
#include "FileDialog.h"

#include <QDir>
#include <QJsonArray>
#include <QMessageBox>

ProjectData::ProjectData() {
    imagesJson = QJsonObject();
    m_projectDir = QDir::homePath();
    m_projectFileName = TEMP_PROJECT_FILENAME;
    projectUpdated = false;
}

QString ProjectData::projectPath() {
    return QDir(m_projectDir).filePath(m_projectFileName);
}

QString ProjectData::projectDir() {
    return m_projectDir;
}

QString ProjectData::projectFileName() {
    return m_projectFileName;
}

void ProjectData::updateProjectFile(QString projDir, QString projFileName) {
    m_projectDir = projDir;
    m_projectFileName = projFileName;
}

void ProjectData::updateProjectFile(QString projPath) {
    m_projectDir = QFileInfo(projPath).absolutePath();
    m_projectFileName = QFileInfo(projPath).fileName();
}

QMap<QString, int> ProjectData::allTagsCount() {
    QMap<QString, int> tagCount;
    for (auto it = imagesJson.constBegin(); it != imagesJson.constEnd(); ++it) {
        QJsonObject imageData = it.value().toObject();
        QJsonArray selectionList = imageData["selection-list"].toArray();

        QJsonArray tagList;
        for (int i = 0; i < selectionList.size(); ++i) {
            QJsonObject selectionObj = selectionList.at(i).toObject();
            tagList = selectionObj["tags"].toArray();
        }

        for (int i = 0; i < tagList.size(); ++i) {
            const QJsonValue& value = tagList.at(i);
            if (!value.isString()) {
                continue;
            }
            QString strVal = value.toString();
            if (!tagCount.contains(strVal)) {
                tagCount[strVal] = 1;
            } else {
                tagCount[strVal] += 1;
            }
        }
    }
    return tagCount;
}

QJsonObject ProjectData::allTagsCountJson() {
    QMap<QString, int> tagCount = allTagsCount();
    QJsonObject tagCountJson;
    for (auto it = tagCount.begin(); it != tagCount.end(); ++it) {
        tagCountJson[it.key()] = it.value();
    }
    return tagCountJson;
}

// Clean project before next action
// If there are unsaved changes -> ask user what to do.
int ProjectData::clean(QString action) {
    if (projectUpdated) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr,
            "Confirm",
            "You're trying to open another project\n"
            "But you have unsaved changes, would you like to save them?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );
        if (reply == QMessageBox::Yes) {
            if (saveWithDialog() != 0) {
                // Something went wrong during saving. Do not remove data.
                return -1;
            }
            imagesJson = QJsonObject();
            projectUpdated = false;
        } else if (reply == QMessageBox::No) {
            // The user chose not to save the changes.
            imagesJson = QJsonObject();
            projectUpdated = false;
        } else if (reply == QMessageBox::Cancel) {
            // User has changed mind - do nothing.
            return -1;
        }
    } else {
        // Project was not updated since last save
        // Should be safe to just remove the data
        imagesJson = QJsonObject();
        projectUpdated = false;
    }
    return 0;
}

int ProjectData::save() {
    QString projPath = projectPath();
    QJsonObject rootJson = QJsonObject();

    rootJson["tags-frequency"] = allTagsCountJson();
    rootJson["project-version"] = PROJECT_VERSION;
    rootJson["project-directory"] = m_projectDir;
    rootJson["images"] = imagesJson;

    QFile file(projPath);
    QJsonDocument doc(rootJson);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // or Compact
        projectUpdated = false;
        file.close();
        return 0;
    }
    qWarning() << "Could not open file for writing:" << file.errorString();
    return -1;
}

int ProjectData::saveWithDialog() {
    if (m_projectFileName == TEMP_PROJECT_FILENAME || !QDir(m_projectDir).exists()) {
        QString fileName = saveProjectDialog(nullptr, m_projectDir);
        if (fileName.isEmpty()) {
            return -1;
        }
        updateProjectFile(fileName);
    }
    return save();
}

int ProjectData::open() {
    if (clean() != 0) {
        return -1;
    }
    QString projPath = projectPath();
    QJsonObject rootJson = QJsonObject();
    imagesJson = QJsonObject();
    projectUpdated = false;
    QFile file(projPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file:" << file.errorString();
        return -1;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (doc.isNull()) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return -2;
    }

    if (!doc.isObject()) {
        qWarning() << "JSON is not an object";
        return -3;
    }

    rootJson = doc.object();
    if (rootJson.contains("images")) {
        imagesJson = rootJson["images"].toObject();
    }
    return 0;
}

int ProjectData::openWithDialog() {
    if (clean() != 0) {
        return -1;
    }
    QString fileName = openProjectDialog(nullptr, m_projectDir);
    if (fileName.isEmpty()) {
        return -1;
    }
    updateProjectFile(fileName);
    return open();
}

int ProjectData::createWithDialog() {
    if (clean() != 0) {
        return -1;
    }
    QString fileName = createProjectDialog(nullptr, m_projectDir);
    if (fileName.isEmpty()) {
        return -1;
    }
    updateProjectFile(fileName);
    return save();
}

QStringList ProjectData::openImagesWithDialog() {
    QStringList fileList = QStringList();
    if (clean() != 0) {
        return fileList;
    }
    fileList = openImagesDialog(nullptr, m_projectDir);
    if (!fileList.isEmpty()) {
        QString newProjectDir = QFileInfo(fileList[0]).absolutePath();
        updateProjectFile(newProjectDir, TEMP_PROJECT_FILENAME);
    }
    return fileList;
}

int ProjectData::openDir(QString dirPath) {
    if (clean() != 0) {
        return -1;
    }
    if (dirPath.isEmpty() || !QDir(dirPath).exists()) {
        return -1;
    }
    updateProjectFile(dirPath, TEMP_PROJECT_FILENAME);
    return 0;
}

int ProjectData::openDirWithDialog() {
    if (clean() != 0) {
        return -1;
    }
    QString dirPath = openDirDialog(nullptr, m_projectDir);
    if (dirPath.isEmpty()) {
        return -1;
    }
    updateProjectFile(dirPath, TEMP_PROJECT_FILENAME);
    return 0;
}

void ProjectData::updateImageData(QString id, QJsonObject data, int selectionCount) {
    if (selectionCount == 0) {
        if (imagesJson.contains(id)) {
            imagesJson.remove(id);
            projectUpdated = true;
        }
        return;
    }
    if (!id.isEmpty()) {
        if (data.contains("image-path")) {
            QString imagePath = data["image-path"].toString();
            QString relativePath = QDir(m_projectDir).relativeFilePath(imagePath);
            data["relative-path"] = relativePath;
        }
        if (data != imagesJson[id].toObject()) {
            projectUpdated = true;
        }
        imagesJson[id] = data;
    }
}

QJsonObject ProjectData::getImageData(QString id) {
    if (imagesJson.contains(id)) {
        return imagesJson[id].toObject();
    }
    return QJsonObject();
}
