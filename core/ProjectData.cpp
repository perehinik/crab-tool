#include "ProjectData.h"
#include "Constants.h"
#include "FileDialog.h"
#include "Checksum.h"

#include <QDir>
#include <QJsonArray>
#include <QMessageBox>

ProjectData::ProjectData() {
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
    emit statusUpdate({projectPath(), isSaved()});
}

void ProjectData::updateProjectFile(QString projPath) {
    QFileInfo fInfo = QFileInfo(projPath);
    updateProjectFile(fInfo.absolutePath(), fInfo.fileName());
}

void ProjectData::setProjectUpdated(bool state) {
    projectUpdated = state;
    emit statusUpdate({projectPath(), isSaved()});
}

bool ProjectData::isSaved() {
    return !projectUpdated;
}

QStringList ProjectData::allTags() {
    QStringList tags;
    for (auto it = imageMap.constBegin(); it != imageMap.constEnd(); ++it) {
        ImageData *imageData = it.value();

        for (int i = 0; i < imageData->selectionList.size(); ++i) {
            const QStringList &tagList = imageData->selectionList.at(i)->tags;

            for (int i = 0; i < tagList.length(); ++i) {
                if (!tags.contains(tagList[i])) {
                    tags.append(tagList[i]);
                }
            }
        }
    }
    return tags;
}

QMap<QString, int> ProjectData::allTagsCount() {
    QMap<QString, int> tagCount;
    for (auto it = imageMap.constBegin(); it != imageMap.constEnd(); ++it) {
        ImageData *imageData = it.value();

        for (int i = 0; i < imageData->selectionList.size(); ++i) {
            const QStringList &tagList = imageData->selectionList.at(i)->tags;

            for (int i = 0; i < tagList.length(); ++i) {
                if (!tagCount.contains(tagList[i])) {
                    tagCount[tagList[i]] = 1;
                } else {
                    tagCount[tagList[i]] += 1;
                }
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
int ProjectData::clearQuery(QString action) {
    if (projectUpdated) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr,
            "Confirm",
            action + "\n"
            "You have unsaved changes, would you like to save them?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );
        if (reply == QMessageBox::Yes) {
            if (saveWithDialog() != 0) {
                // Something went wrong during saving. Do not remove data.
                return -1;
            }
        } else if (reply == QMessageBox::No) {
            return 0;
        } else if (reply == QMessageBox::Cancel) {
            // User has changed mind - do nothing.
            return -1;
        }
    }
    return 0;
}

void ProjectData::clear() {
    for (auto it = imageMap.constBegin(); it != imageMap.constEnd(); ++it) {
        delete it.value();
    }
    imageMap.clear();
    setProjectUpdated(false);
}

ProjectData::~ProjectData() {
    clear();
}

int ProjectData::save() {
    QString projPath = projectPath();
    QJsonObject rootJson = QJsonObject();
    QJsonObject imagesJson = QJsonObject();
    for (auto it = imageMap.constBegin(); it != imageMap.constEnd(); ++it) {
        ImageData *imageData = it.value();
        if (!imageData->hash.isEmpty() && imageData->selectionCount() > 0) {
            imagesJson[imageData->hash] = imageData->toJson();
        }
    }

    rootJson["tags-frequency"] = allTagsCountJson();
    rootJson["project-version"] = PROJECT_VERSION;
    rootJson["project-directory"] = m_projectDir;
    rootJson["images"] = imagesJson;

    QFile file(projPath);
    QJsonDocument doc(rootJson);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // or Compact
        setProjectUpdated(false);
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
    if (clearQuery("Opening project") != 0) {
        return -1;
    }
    QString projPath = projectPath();
    QJsonObject rootJson = QJsonObject();
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
    if (!rootJson.contains("images") || !rootJson["images"].isObject()) {
        return -4;
    }

    QJsonObject imagesJson = rootJson["images"].toObject();
    for (auto it = imagesJson.constBegin(); it != imagesJson.constEnd(); ++it) {
        if (!it.value().isObject()) {
            continue;
        }
        imageMap[it.key()] = new ImageData(m_projectDir, it.value().toObject());
    }
    return 0;
}

int ProjectData::openWithDialog() {
    if (clearQuery("Opening project") != 0) {
        return -1;
    }
    QString fileName = openProjectDialog(nullptr, m_projectDir);
    if (fileName.isEmpty()) {
        return -1;
    }
    clear();
    updateProjectFile(fileName);
    return open();
}

int ProjectData::createWithDialog() {
    if (clearQuery("Creating project") != 0) {
        return -1;
    }
    QString fileName = createProjectDialog(nullptr, m_projectDir);
    if (fileName.isEmpty()) {
        return -1;
    }
    clear();
    updateProjectFile(fileName);
    return save();
}

QStringList ProjectData::openImagesWithDialog() {
    QStringList fileList = QStringList();
    if (clearQuery("Opening images") != 0) {
        return fileList;
    }
    fileList = openImagesDialog(nullptr, m_projectDir);
    if (!fileList.isEmpty()) {
        clear();
        QString newProjectDir = QFileInfo(fileList[0]).absolutePath();
        updateProjectFile(newProjectDir, TEMP_PROJECT_FILENAME);
    }
    return fileList;
}

int ProjectData::openDir(QString dirPath) {
    if (clearQuery("Opening directory") != 0) {
        return -1;
    }
    if (dirPath.isEmpty() || !QDir(dirPath).exists()) {
        return -1;
    }
    clear();
    updateProjectFile(dirPath, TEMP_PROJECT_FILENAME);
    return 0;
}

int ProjectData::openDirWithDialog() {
    if (clearQuery("Opening directory") != 0) {
        return -1;
    }
    QString dirPath = openDirDialog(nullptr, m_projectDir);
    if (dirPath.isEmpty()) {
        return -1;
    }
    clear();
    updateProjectFile(dirPath, TEMP_PROJECT_FILENAME);
    return 0;
}

QStringList ProjectData::keys() {
    return imageMap.keys();
}

ImageData * ProjectData::getImageDataById(QString hash) {
    return imageMap[hash];
}

ImageData * ProjectData::getImageData(QString imagePath) {
    QString hash = sha256(imagePath);
    ImageData *imgData;
    if (!imageMap.contains(hash)) {
        imgData = new ImageData(m_projectDir, imagePath);
        imageMap[hash] = imgData;

    } else {
        imgData = imageMap[hash];
    }
    return imgData;
}
