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
    rectangleList.clear();
    this->setImage(projectPath, imagePath);
}

ImageData::ImageData(QString projectPath, const QJsonObject &json) {
    rectangleList.clear();
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
    for (int i = 0; i < rectangleList.length(); ++i) {
        if (!rectangleList[i]) {
            continue;
        }
        if (rectangleList[i]) {
            rectArray.append(rectangleList[i]->toJson());
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
        rectangleList.append(new SelectionRect(jsonObj));
    }
}

void ImageData::addToScene(QGraphicsScene *scene) {
    for (int i = 0; i < rectangleList.length(); ++i) {
        rectangleList[i]->addToScene(scene);
    }
}

void ImageData::removeFromScene() {
    for (int i = 0; i < rectangleList.length(); ++i) {
        rectangleList[i]->removeFromScene();
    }
}

void ImageData::setScale(qreal scale) {
    for (int i = 0; i < rectangleList.length(); ++i) {
        rectangleList[i]->setScale(scale);
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
    for (int i = 0; i < rectangleList.size(); ++i) {
        if (rectangleList[i] == rect) {
            if (!deleted) {
                rectangleList[i]->removeFromScene();
                delete rectangleList[i];
                deleted = true;
            }
            rectangleList[i] = nullptr;
        }
    }
    rectangleList.removeAll(nullptr);
}

void ImageData::updateRectScale(qreal scale) {
    for (int i = 0; i < rectangleList.size(); ++i) {
        rectangleList[i]->setScale(scale);
    }
}

void ImageData::appendSelection(SelectionRect *rect) {
    rectangleList.append(rect);
}

bool ImageData::containsSelection(SelectionRect *rect) {
    return rectangleList.contains(rect);
}

int ImageData::selectionCount() {
    return rectangleList.length();
}

void ImageData::clear() {
    this->imageRelativePath.clear();
    this->projectPath.clear();
    this->imagePath.clear();
    this->hash.clear();
    for (int i = 0; i < rectangleList.length(); i++) {
        delete rectangleList[i];
    }
    rectangleList.clear();
}

ImageData::~ImageData() {
    emit aboutToBeDeleted();
    clear();
}
