#include "ImageDirOnlyProxy.h"
#include <QFileSystemModel>

bool ImageDirOnlyProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(sourceModel());
    if (!fsModel) {
        return false;
    }

    QModelIndex index = fsModel->index(sourceRow, 0, sourceParent);
    QFileInfo info = fsModel->fileInfo(index);
    if (!info.isDir()) {
        return false;
    }

    if (!info.absoluteFilePath().startsWith(fsModel->rootPath())) {
        return false;
    }

    return directoryContainsImages(info.absoluteFilePath());
}

bool ImageDirOnlyProxy::hasChildren(const QModelIndex &parent) const {
    QModelIndex sourceParent = mapToSource(parent);
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(sourceModel());
    if (!fsModel) return false;

    QFileInfo parentInfo = fsModel->fileInfo(sourceParent);
    if (!parentInfo.isDir()) return false;

    QDir dir(parentInfo.absoluteFilePath());
    QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < subDirs.length(); i++) {
        if (directoryContainsImages(subDirs[i].absoluteFilePath()))
            return true;
    }
    return false;
}

bool ImageDirOnlyProxy::directoryContainsImages(const QString &path) const {
    if (imageDirCache.contains(path)) {
        return imageDirCache[path];
    }

    QDir dir(path);
    QStringList imageFilters = {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif", "*.webp"};

    if (!dir.entryList(imageFilters, QDir::Files).isEmpty()) {
        imageDirCache[path] = true;
        return true;
    }

    QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < subDirs.length(); i++) {
        if (directoryContainsImages(subDirs[i].absoluteFilePath())) {
            imageDirCache[path] = true;
            return true;
        }
    }

    imageDirCache[path] = false;
    return false;
}
