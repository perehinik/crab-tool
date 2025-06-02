#ifndef IMAGENAVIGATORWIDGET_H
#define IMAGENAVIGATORWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QListView>
#include "ImageListModel.h"

#define PIXMAP_MAX_SIDE_SIZE 300

class ImageNavigatorWidget : public QWidget {
    Q_OBJECT

private:
    QString dirPath;
    QList<QString> fileNameList;
    QList<QString> filePathList;
    QList<QPixmap> pixmapList;
    ImageListModel *model;
    QListView *view;

    void onImageItemClicked(const QModelIndex &index);

public:
    explicit ImageNavigatorWidget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void updateItems(QString dirPath);
    void setPath(QString dirPath);
    void updateItem(int index, const QString filePath, const QPixmap &pixmap);
    void updateView();

signals:
    void onImageClicked(QString imagePath);
};

#endif // IMAGENAVIGATORWIDGET_H
