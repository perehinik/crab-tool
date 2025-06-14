#ifndef IMAGEITEMDELEGATE_H
#define IMAGEITEMDELEGATE_H

#include <QStyledItemDelegate>

class ImageItemDelegate : public QStyledItemDelegate {
public:
    QWidget *parentView;

    ImageItemDelegate(QWidget *parent) : QStyledItemDelegate(parent), parentView(parent) {}

    void paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // IMAGEITEMDELEGATE_H
