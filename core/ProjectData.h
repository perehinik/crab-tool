#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QString>
#include <QJsonObject>

struct ProjectStatus {
    QString projectPath;
    bool isSaved;
};

class ProjectData : public QObject {
    Q_OBJECT

public:
    ProjectData();
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
    QJsonObject getImageData(QString id);
    bool isSaved();
    QStringList allTags();

private:
    QString m_projectDir;
    QString m_projectFileName;

    QJsonObject imagesJson;
    bool projectUpdated;
    void updateProjectFile(QString projPath);
    void updateProjectFile(QString projDir, QString projFileName);
    void setProjectUpdated(bool state);

signals:
    void statusUpdate(const ProjectStatus status);
};

#endif // PROJECTDATA_H
