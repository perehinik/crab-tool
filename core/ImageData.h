#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QJsonObject>
#include "SelectionRect.h"

class ImageData {
public:
    explicit ImageData(QString projPath, QString imagePath);
    explicit ImageData(QJsonObject jsonObject);

QString imagePath;
QString hash;

void setImage(QString projPath, QString imagePath);
void setZoom(double newZoomLevel);
QJsonObject toJson();
void fromJson(const QJsonObject &json);
int selectionCount();
QList<SelectionRect*> rectangleList;
};

#endif // IMAGEDATA_H
