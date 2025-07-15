#ifndef EXPORTHAAR_H
#define EXPORTHAAR_H

#include "ExportBase.h"
#include <QThreadPool>
#include <QMutex>

class ExportHaar : public ExportBase {
    Q_OBJECT

public:
    ExportHaar(ProjectData *data, QWidget *parent = nullptr);

private:
    void exportAndSave() override;
    void exportImageData(ImageData *imgData, const QStringList &positives, const QStringList &negatives);
    void savePositive(QString tag, QString imgPath, QList<SelectionRect*> &selList);
    void saveNegative(QString tag, QString imgPath, QList<SelectionRect*> &selList);

    QListWidget *tagListPositives;
    QListWidget *tagListNegatives;
    QStringList selectedTagsPositive() const;
    QStringList selectedTagsNegative() const;
    QThreadPool *exportThreadPool;
    QMutex fileWriteMutex;
};

#endif // EXPORTHAAR_H
