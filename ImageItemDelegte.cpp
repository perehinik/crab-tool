#include "ImageItemDelegate.h"
#include <QPainter>

void ImageItemDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const {
    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    QString text = index.data(Qt::DisplayRole).toString();

    p->save();

    QRect r = opt.rect;
    qreal scalingFactor = pixmap.width() / ( r.width() * 0.8 );
    int imageWidth = (qreal)pixmap.width() / scalingFactor;
    int imageHeight = (qreal)pixmap.height() / scalingFactor;  // Maintain aspect ratio (adjust as needed)

    QRect imageRect(r.x() + (r.width() - imageWidth) / 2, r.y(), imageWidth, imageHeight);
    QRect textRect(r.x(), imageRect.bottom() + 5, r.width(), r.height() - imageHeight - 5);

    p->drawPixmap(imageRect, pixmap.scaled(imageRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    p->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, text);

    p->restore();
}

QSize ImageItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

    qreal scalingFactor = pixmap.width() / ( parentView->width() * 0.8 );
    int imageHeight = (qreal)pixmap.height() / scalingFactor;
    int width = parentView->width();  // Full width of QListView
    int height = imageHeight + 30;  // image height + text
    return QSize(width, height);
}
