#include <QPalette>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

#include "ImageData.h"
#include "Checksum.h"

ImageData::ImageData(QString projectPath, QString imagePath) {
    selectionList.clear();
    this->setImage(projectPath, imagePath);
}

ImageData::ImageData(QString projectPath, const QJsonObject &json) {
    selectionList.clear();
    this->fromJson(projectPath, json);
}

QJsonObject ImageData::toJson() {
    QJsonObject obj;

    obj["type"] = "image-data";
    obj["image-path"] = imagePath;
    obj["relative-path"] = imageRelativePath;
    obj["image-filename"] = QFileInfo(imagePath).fileName();
    obj["sha256"] = hash;

    QJsonArray rectArray;
    for (int i = 0; i < selectionList.length(); ++i) {
        if (!selectionList[i]) {
            continue;
        }
        if (selectionList[i]) {
            rectArray.append(selectionList[i]->toJson());
        }
    }
    obj["selection-list"] = rectArray;
    return obj;
}

void ImageData::fromJson(QString projectPath, const QJsonObject &json) {
    if (json["type"].toString() != "image-data") {
        return;
    }
    // Try to load image if checksum is not the same as in project/JSON
    if (json["sha256"].toString() != hash) {
        QString relativePath = json["relative-path"].toString();
        QString fullPath = QDir(projectPath).absoluteFilePath(relativePath);
        setImage(projectPath, fullPath);
    }
    // Check if loaded image hash is the same as in project.
    if (json["sha256"].toString() != hash) {
        QMessageBox::critical(nullptr, "Error", "Image checksum is not the same as in project!\nFile: " + imagePath);
        clear();
    }

    QJsonArray rectArray = json["selection-list"].toArray();
    for (int i = 0; i < rectArray.size(); ++i) {
        QJsonObject jsonObj = rectArray.at(i).toObject();
        selectionList.append(new SelectionRect(jsonObj));
    }
}

void ImageData::addToScene(QGraphicsScene *scene) {
    for (int i = 0; i < selectionList.length(); ++i) {
        selectionList[i]->addToScene(scene);
    }
}

void ImageData::removeFromScene() {
    for (int i = 0; i < selectionList.length(); ++i) {
        selectionList[i]->removeFromScene();
    }
}

void ImageData::setScale(qreal scale) {
    for (int i = 0; i < selectionList.length(); ++i) {
        selectionList[i]->setScale(scale);
    }
    previousScale = scale;
}

qreal ImageData::getScale() {
    return previousScale;
}

void ImageData::setImage(QString projectPath, QString imagePath) {
    clear();
    hash = sha256(imagePath);
    this->imageRelativePath = QDir(projectPath).relativeFilePath(imagePath);
    this->projectPath = projectPath;
    this->imagePath = imagePath;

    if (hash.isEmpty()) {
        QMessageBox::critical(nullptr, "Error", "Image checksum calculation error!\nFile: " + imagePath);
    }
}

void ImageData::deleteRect(SelectionRect *rect) {
    bool deleted = false;
    for (int i = 0; i < selectionList.size(); ++i) {
        if (selectionList[i] == rect) {
            if (!deleted) {
                selectionList[i]->removeFromScene();
                delete selectionList[i];
                deleted = true;
            }
            selectionList[i] = nullptr;
        }
    }
    selectionList.removeAll(nullptr);
}

QStringList ImageData::tags() {
    QStringList tagList;
    for (int i = 0; i < selectionList.size(); ++i) {
        const QStringList &tagListSelection = selectionList[i]->tags;

        for (int i = 0; i < tagListSelection.length(); ++i) {
            if (!tagList.contains(tagListSelection[i])) {
                tagList.append(tagListSelection[i]);
            }
        }
    }
    return tagList;
}

void ImageData::getSelectionsByTag(QString tag, QList<SelectionRect*> &selList) {
    selList.clear();
    for (int selectionId = 0; selectionId < selectionCount(); selectionId++) {
        SelectionRect *selection = selectionList[selectionId];
        for (int tagId = 0; tagId < selection->tags.count(); ++tagId) {
            if (selection->tags[tagId] == tag) {
                selList.append(selection);
            }
        }
    }
}

void ImageData::updateRectScale(qreal scale) {
    for (int i = 0; i < selectionList.size(); ++i) {
        selectionList[i]->setScale(scale);
    }
}

void ImageData::appendSelection(SelectionRect *rect) {
    selectionList.append(rect);
}

bool ImageData::containsSelection(SelectionRect *rect) {
    return selectionList.contains(rect);
}

int ImageData::selectionCount() {
    return selectionList.length();
}

void ImageData::clear() {
    this->imageRelativePath.clear();
    this->projectPath.clear();
    this->imagePath.clear();
    this->hash.clear();
    for (int i = 0; i < selectionList.length(); i++) {
        delete selectionList[i];
    }
    selectionList.clear();
}

ImageData::~ImageData() {
    emit aboutToBeDeleted();
    clear();
}
