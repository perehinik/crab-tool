#ifndef EXPORTYOLO_H
#define EXPORTYOLO_H

#include "ExportBase.h"

class ExportYolo : public ExportBase {
    Q_OBJECT

public:
    ExportYolo(ProjectData *data, QWidget *parent = nullptr);
    QListWidget *tagList;
    QStringList selectedTags() const;
    QString getYoloDescriptor(ImageData *imgData, const QStringList &tags);
    void clearTagsCount(const QStringList &tags);
    bool useForVerif(const QStringList &tags, const QString &imageFileName);
    void updateSavedTagCount(const QStringList &tags, bool isVerif);

private:
    QMap<QString, qreal> trainTagsCount;
    QMap<QString, qreal> verifTagsCount;

    void exportAndSave() override;
    void saveYaml(const QString &trainPath, const QString &valPath, const QStringList &tags);
};

#endif // EXPORTYOLO_H
