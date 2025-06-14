#include <QDirIterator>
#include <QFileSystemModel>
#include <QThreadPool>
#include <QImageReader>

#include "Constants.h"
#include "ImageItemDelegate.h"
#include "ImageNavigatorWidget.h"

ImageNavigatorWidget::ImageNavigatorWidget(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumWidth(180);

    QLayout *mainLayout = new QGridLayout();
    setLayout(mainLayout);

    model = new ImageListModel;
    model->defaultPixmap = QPixmap(":/icon/image-wide.png");

    view = new QListView;
    view->setModel(model);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    view->setItemDelegate(new ImageItemDelegate(view));
    view->setSpacing(0);
    view->setUniformItemSizes(false);  // Needed for custom height
    view->setSelectionMode(QAbstractItemView::NoSelection);
    view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(view, &QListView::clicked, this, &ImageNavigatorWidget::onImageItemClicked);

    mainLayout->addWidget(view);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void ImageNavigatorWidget::setPath(QString dirPath) {
    this->dirPath = dirPath;
    QStringList imagePathList;

    QDirIterator it(dirPath, IMAGE_EXTENSION_FILTERS, QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        imagePathList << filePath;
    }
    loadItems(imagePathList);
}

void ImageNavigatorWidget::loadItems(QStringList imagePathList) {
    QPointer<ImageNavigatorWidget> self = this;

    QStringList imageNameList;

    imagePathList.sort();
    for (int i = 0; i < imagePathList.length(); i ++) {
        imageNameList << QFileInfo(imagePathList[i]).fileName();
    }

    QList<QPixmap> pixmapListLocal(imagePathList.size());

    filePathList = imagePathList;
    fileNameList = imageNameList;
    pixmapList = pixmapListLocal;
    model->updateData(imageNameList, pixmapListLocal);
    view->viewport()->update();

    for (int i = 0; i < imagePathList.length(); i++) {
        QThreadPool::globalInstance()->start([=]() {
            QString imagePath = imagePathList[i];
            // First load scaled image with fast transformation
            // Resolution should be a bit better than one that will be finally used
            QImageReader reader(imagePath);
            QSizeF size = reader.size();
            qreal imageScale = size.height() / size.width();

            qreal maxOrigSize = std::max(size.height(), size.width());
            qreal resizeOrigFactor = maxOrigSize / (PIXMAP_MAX_SIDE_SIZE * 2);
            if (resizeOrigFactor < 1) { resizeOrigFactor = 1; }

            reader.setScaledSize(QSize(size.width() / resizeOrigFactor, size.height() / resizeOrigFactor));  // Load directly as 64×64 image
            QImage lowResImage = reader.read();
            QPixmap pixmap = QPixmap::fromImage(lowResImage);
            if (!pixmap) {
                pixmap = QPixmap(":/icon/image-broken-wide.png");
            }

            // Rescale image with smooth transform
            qreal maxSize = std::max(pixmap.height(), pixmap.width());
            qreal resizeFactor = maxSize / PIXMAP_MAX_SIDE_SIZE;
            pixmap = pixmap.scaled(QSize(pixmap.width() / resizeFactor, pixmap.height() / resizeFactor),
                                               Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QMetaObject::invokeMethod(this, [=]() {
                if (!self) {
                    return; // If object in main thread does not exist anymore
                }
                this->updateItem(i, imagePath, pixmap);
            }, Qt::QueuedConnection);

            // This should be done different way, I should check if all pixmaps got loaded before update.
            if (i + 1 >= imagePathList.length()) {
                QMetaObject::invokeMethod(this, [=]() {
                    if (!self) {
                        return; // If object in main thread does not exist anymore
                    }
                    this->updateView();
                }, Qt::QueuedConnection);
            }
        });
    }
}

void ImageNavigatorWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateView();
}

void ImageNavigatorWidget::onImageItemClicked(const QModelIndex &index) {
    emit onImageClicked(filePathList[index.row()]);
}

void ImageNavigatorWidget::updateItem(int index, const QString filePath, const QPixmap &pixmap) {
    if (pixmapList.isEmpty() || pixmapList.length() <= index) {
        return;
    }
    int topIndex = view->indexAt(QPoint(0, 0)).row();
    int bottomIndex = view->indexAt(QPoint(0, view->viewport()->height() - 1)).row();
    filePathList[index] = filePath;
    fileNameList[index] = QFileInfo(filePath).fileName();
    pixmapList[index] = pixmap;
    model->fileNameList[index] = fileNameList[index];
    model->pixmapList[index] = pixmapList[index];
    if ((index <= bottomIndex && index >= topIndex) || index % 100 == 0) {
        updateView();
    }
}

void ImageNavigatorWidget::updateView() {
    if (!view || !view->model()) {
        return;
    }
    view->doItemsLayout();  // forces layout update
    view->updateGeometry();
}
