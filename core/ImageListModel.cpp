#include "ImageListModel.h"

int ImageListModel::rowCount(const QModelIndex &) const {
    return fileNameList.size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    int row = index.row();

    if (role == Qt::DisplayRole) {
        return fileNameList.value(row);
    } else if (role == Qt::DecorationRole) {
        if (row < pixmapList.size() && !pixmapList[row].isNull())
            return pixmapList[row];
        else
            return defaultPixmap;
    }

    return QVariant();
}

void ImageListModel::updateData(const QStringList &newFiles, const QList<QPixmap> &newPixmaps) {
    beginResetModel();  // Notify view that model is being reset
    fileNameList = newFiles;
    pixmapList = newPixmaps;
    endResetModel();    // Notify view to refresh all items
}
