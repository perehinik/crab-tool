#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QJsonObject>
#include "SelectionRect.h"

class ImageData : public QObject {
    Q_OBJECT

public:
    explicit ImageData(QString projectPath, QString imagePath);
    explicit ImageData(QString projectPath, const QJsonObject &json);

    void setImage(QString projectPath, QString imagePath);
    ~ImageData();
    QJsonObject toJson();
    void fromJson(QString projectPath, const QJsonObject &json);
    int selectionCount();
    SelectionRect * getRectByPoint(QPointF point);
    void updateRectScale(qreal scale);
    void clear();
    void appendSelection(SelectionRect *rect);
    bool containsSelection(SelectionRect *rect);

    void addToScene(QGraphicsScene *scene);
    void deleteRect(SelectionRect *rect);
    void removeFromScene();
    void setScale(qreal scale);
    qreal getScale();

    QString imagePath;
    QString imageRelativePath;
    QString projectPath;
    QString hash;
    QList<SelectionRect*> rectangleList;

private:
    qreal previousScale = 6;


signals:
    void aboutToBeDeleted();
};

#endif // IMAGEDATA_H
