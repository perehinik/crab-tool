#include "ImageFilterProxy.h"
#include <QFileSystemModel>

bool ImageFilterProxy :: filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QStringList imageExtensions = {"jpg", "jpeg", "png", "bmp", "gif", "webp"};
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(sourceModel());
    if (!fsModel) {
        return false;
    }
    QFileInfo info = fsModel->fileInfo(index);
    if (info.isDir()) {
        return true;  // Always allow directories
    }
    QString suffix = info.suffix().toLower();

    return imageExtensions.contains(suffix, Qt::CaseInsensitive);
}
