#include "file_manager_widget.h"
#include "file_manager_dir_widget.h"
#include <QDirIterator>
#include <QFileSystemModel>
#include <QListView>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <qimagereader.h>

class ImageFilterProxy : public QSortFilterProxyModel {
public:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(sourceModel());
        if (!fsModel) return false;

        QFileInfo info = fsModel->fileInfo(index);

        if (info.isDir()) return true;  // Always allow directories

        QString suffix = info.suffix().toLower();
        return suffix == "jpg" || suffix == "jpeg" || suffix == "png" || suffix == "bmp" || suffix == "gif" || suffix == "webp";
    }
};

class ImageDirOnlyProxy : public QSortFilterProxyModel {
public:
    ImageDirOnlyProxy(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {}

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(sourceModel());
        if (!fsModel) return false;

        QFileInfo info = fsModel->fileInfo(index);
        if (!info.isDir()) return false;

        return directoryContainsImages(info.absoluteFilePath());
    }

    bool hasChildren(const QModelIndex &parent) const override {
        QModelIndex sourceParent = mapToSource(parent);
        QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(sourceModel());
        if (!fsModel) return false;

        QFileInfo parentInfo = fsModel->fileInfo(sourceParent);
        if (!parentInfo.isDir()) return false;

        QDir dir(parentInfo.absoluteFilePath());
        qDebug() << parentInfo.absoluteFilePath();
        QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &subdir : subDirs) {
            if (directoryContainsImages(subdir.absoluteFilePath()))
                return true;
        }
        return false;
    }

private:
    bool directoryContainsImages(const QString &path) const {
        QDir dir(path);
        QStringList imageFilters = {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif", "*.webp"};

        if (!dir.entryList(imageFilters, QDir::Files).isEmpty())
            return true;

        QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &subdir : subDirs) {
            if (directoryContainsImages(subdir.absoluteFilePath()))
                return true;
        }

        return false;
    }
};

FileManager::FileManager(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumWidth(180);

    containerWidget = new QWidget(this);
    containerLayout = new QGridLayout();
    containerLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);

    containerWidget->setLayout(containerLayout);

    scrollArea = new QScrollArea();
    scrollArea->setAttribute(Qt::WA_OpaquePaintEvent);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //updateItems("/home/ivan/proj/TrainingData/RAW/");

    QLayout *mainLayout = new QGridLayout();
    setLayout(mainLayout);


    QFileSystemModel *fsModel = new QFileSystemModel;
    fsModel->setRootPath(QDir::homePath());

    ImageDirOnlyProxy *proxy = new ImageDirOnlyProxy;
    proxy->setSourceModel(fsModel);

    QTreeView *tree = new QTreeView;
    tree->setMaximumHeight(200);
    tree->setModel(proxy);
    tree->setRootIndex(proxy->mapFromSource(fsModel->index("/home/ivan/proj/TrainingData/")));  // Start at home directory

    tree->setHeaderHidden(true);  // Optional: hide file size/date columns
    tree->setColumnHidden(1, true); // Size
    tree->setColumnHidden(2, true); // File type
    tree->setColumnHidden(3, true); // Date modified

    connect(tree, &QTreeView::doubleClicked, this, [=](const QModelIndex &index) {
        QModelIndex sourceIndex = proxy->mapToSource(index);  // ✅ Convert proxy index to source index
        QString path = fsModel->filePath(sourceIndex);
        updateItems(path);
    });


    mainLayout->addWidget(tree);

    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void FileManager::updateItems(QString dirPath) {
    int i = 0;
    for(int w=0; w < buttonList.length(); w ++) {
        buttonList[w]->hide();
        buttonList[w]->deleteLater();
    }
    buttonList.clear();
    QDirIterator it(dirPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString subdirPath = it.next();
        FileManagerDir *fileManagerDir = new FileManagerDir(subdirPath, containerWidget);
        containerLayout->addWidget(fileManagerDir, i, 0);
        QObject::connect(fileManagerDir, &FileManagerDir::onImageButtonClicked, this, &FileManager::onButtonPressed);
        buttonList.append(fileManagerDir);
        i++;
    }
    if (i == 0) {
        FileManagerDir *fileManagerDir = new FileManagerDir(dirPath, containerWidget);
        containerLayout->addWidget(fileManagerDir, i, 0);
        QObject::connect(fileManagerDir, &FileManagerDir::onImageButtonClicked, this, &FileManager::onButtonPressed);
        buttonList.append(fileManagerDir);
    }
}

void FileManager::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void FileManager::onButtonPressed(QString imagePath) {
    emit onImageButtonClicked(imagePath);
}
