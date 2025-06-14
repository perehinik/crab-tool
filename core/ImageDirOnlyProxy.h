#ifndef IMAGEDIRONLYPROXY_H
#define IMAGEDIRONLYPROXY_H

#include <QSortFilterProxyModel>

// Proxy for directories containing images or subdirectories with images.
// All other directories skipped

class ImageDirOnlyProxy : public QSortFilterProxyModel {
public:
    ImageDirOnlyProxy(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool hasChildren(const QModelIndex &parent) const override;

private:
    mutable QHash<QString, bool> imageDirCache;

    bool directoryContainsImages(const QString &path) const;
};

#endif // IMAGEDIRONLYPROXY_H
