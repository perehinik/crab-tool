#ifndef DIRNAVIGATORWIDGET_H
#define DIRNAVIGATORWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include "ImageDirOnlyProxy.h"

class DirNavigatorWidget : public QWidget {
    Q_OBJECT

private:
    QString dirPath;
    QString requestedPath;
    QFileSystemModel *model;
    ImageDirOnlyProxy *proxy;
    QTreeView *view;
    bool initialized;
    void onPathChanged(const QModelIndex &index);
    void showEvent(QShowEvent *event);

public:
    explicit DirNavigatorWidget(QWidget *parent = nullptr);
    void setPath(QString dirPath);

signals:
    void onDirPathChanged(QString imagePath);
};

#endif // DIRNAVIGATORWIDGET_H
