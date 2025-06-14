#include "ImageItemDelegate.h"
#include <QPainter>
#include <QApplication>

void ImageItemDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const {
    QStyleOptionViewItem option(opt);  // make a modifiable copy
    initStyleOption(&option, index);   // ensures proper state flags like selected, hovered

    p->save();

    // Draw selection/highlight background
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, p, option.widget);

    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    QString text = index.data(Qt::DisplayRole).toString();

    QRect r = opt.rect;
    qreal scalingFactor = pixmap.width() / ( r.width() * 0.7 );
    int imageWidth = (qreal)pixmap.width() / scalingFactor;
    int imageHeight = (qreal)pixmap.height() / scalingFactor;  // Maintain aspect ratio (adjust as needed)

    QFont font = p->font();
    int fontSize = QFontInfo(font).pixelSize();
    const int spacing = 5;
    QRect imageRect(r.x() + (r.width() - imageWidth) / 2,
                    r.y() + (r.height() - imageHeight - fontSize - spacing) / 2,
                    imageWidth,
                    imageHeight);
    QRect textRect(r.x(), imageRect.bottom() + spacing, r.width(), r.height() - imageHeight - spacing);

    p->drawPixmap(imageRect, pixmap.scaled(imageRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    p->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, text);

    p->restore();
}

QSize ImageItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

    qreal scalingFactor = pixmap.width() / ( parentView->width() * 0.7 );
    int imageHeight = (qreal)pixmap.height() / scalingFactor;
    int width = parentView->width();  // Full width of QListView
    int height = imageHeight + 30;  // image height + text
    return QSize(width, height);
}
