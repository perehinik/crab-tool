#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QString>
#include <QJsonObject>

#include "ImageData.h"

struct ProjectStatus {
    QString projectPath;
    bool isSaved;
};

class ProjectData : public QObject {
    Q_OBJECT

public:
    ProjectData();
    ~ProjectData();
    int save();
    int saveWithDialog();
    int open();
    int openWithDialog();
    int createWithDialog();
    QStringList openImagesWithDialog();
    int openDir(QString dirPath);
    int openDirWithDialog();
    QString projectPath();
    QString projectDir();
    QString projectFileName();
    void extracted(QMap<QString, int> &tagCount, QJsonArray &tagList);
    QMap<QString, int> allTagsCount();
    QJsonObject allTagsCountJson();
    int clearQuery(QString action = "");
    void clear();
    void updateImageData(QString id, QJsonObject data, int selectionCount);
    ImageData * getImageData(QString imagePath);
    bool isSaved();
    QStringList allTags();

private:
    QMap<QString, ImageData*> imageMap;
    QString m_projectDir;
    QString m_projectFileName;

    bool projectUpdated;
    void updateProjectFile(QString projPath);
    void updateProjectFile(QString projDir, QString projFileName);
    void setProjectUpdated(bool state);

signals:
    void aboutToBeDeleted();
    void statusUpdate(const ProjectStatus status);
};

#endif // PROJECTDATA_H
