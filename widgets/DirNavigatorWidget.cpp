#include <QThread>

#include "DirNavigatorWidget.h"

DirNavigatorWidget::DirNavigatorWidget(QWidget *parent) : QWidget(parent) {
    setMinimumWidth(180);
    setMinimumHeight(100);

    initialized = false;

    QLayout *mainLayout = new QGridLayout(this);

    view = new QTreeView(this);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(view, &QTreeView::clicked, this, &DirNavigatorWidget::onPathChanged);

    mainLayout->addWidget(view);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void DirNavigatorWidget::onPathChanged(const QModelIndex &index) {
    if (!initialized || !index.isValid()) {
        return;
    }
    QModelIndex sourceIndex = proxy->mapToSource(index);  // ✅ Convert proxy index to source index
    QString path = model->filePath(sourceIndex);
    if (path != dirPath) {
        dirPath = path;
        emit onDirPathChanged(path);
    }
}

void DirNavigatorWidget::setPath(QString dirPath) {
    // while (!initialized) {
    //     QThread::msleep(100);
    // }
    requestedPath = dirPath;
    model = new QFileSystemModel(this);
    model->setRootPath(dirPath);

    proxy = new ImageDirOnlyProxy(this);
    proxy->setSourceModel(model);

    view->setModel(proxy);
    view->setRootIndex(proxy->mapFromSource(model->index(dirPath)));
    view->setHeaderHidden(true);  // Optional: hide file size/date columns
    view->setColumnHidden(1, true); // Size
    view->setColumnHidden(2, true); // File type
    view->setColumnHidden(3, true); // Date modified
    initialized = true;
}

void DirNavigatorWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    if (!initialized) {
        initialized = true;
    }
}
