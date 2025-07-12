#ifndef EXPORTHAAR_H
#define EXPORTHAAR_H

#include "ExportBase.h"

class ExportHaar : public ExportBase {
    Q_OBJECT

public:
    ExportHaar(ProjectData *data, QWidget *parent = nullptr);

private:
    void exportAndSave() override;
    void exportPositives(ImageData *imgData, const QStringList &positives);
    void exportNegatives(ImageData *imgData, const QStringList &negatives);
    void savePositive(QString tag, QString imgPath, QList<SelectionRect*> &selList);
    void saveNegative(QString tag, QString imgPath, QList<SelectionRect*> &selList);

    QListWidget *tagListPositives;
    QListWidget *tagListNegatives;
    QStringList selectedTagsPositive() const;
    QStringList selectedTagsNegative() const;
};

#endif // EXPORTHAAR_H
