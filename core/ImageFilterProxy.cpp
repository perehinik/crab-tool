#include <QFileSystemModel>

#include "ImageFilterProxy.h"
#include "Constants.h"

bool ImageFilterProxy :: filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
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

    return IMAGE_EXTENSIONS.contains(suffix, Qt::CaseInsensitive);
}
