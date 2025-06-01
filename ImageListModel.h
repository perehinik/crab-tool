#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QListView>

class ImageListModel : public QAbstractListModel {
public:
    QStringList fileNameList;
    QList<QPixmap> pixmapList;
    QPixmap defaultPixmap;

    ImageListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &)  const override ;
    QVariant data(const QModelIndex &index, int role) const override;
    void updateData(const QStringList &newFiles, const QList<QPixmap> &newPixmaps);
};

#endif // IMAGELISTMODEL_H
