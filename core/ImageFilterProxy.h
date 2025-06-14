#ifndef IMAGEFILTERPROXY_H
#define IMAGEFILTERPROXY_H

#include <QSortFilterProxyModel>

class ImageFilterProxy : public QSortFilterProxyModel {
public:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // IMAGEFILTERPROXY_H
