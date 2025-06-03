#include "ImageNavigatorWidget.h"
#include <QDirIterator>
#include <QFileSystemModel>
#include <QThreadPool>
#include <QImageReader>
#include "ImageItemDelegate.h"

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
    QPointer<ImageNavigatorWidget> self = this;
    this->dirPath = dirPath;

    QThreadPool::globalInstance()->start([=]() {
        QStringList filePathListLocal;
        QStringList fileNameListLocal;
        QPixmap brokenPixmap = QPixmap(":/icon/image-broken-wide.png");

        QDirIterator it(dirPath, QStringList() << "*.jpg" << "*.png", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString filePath = it.next();
            filePathListLocal << filePath;
            fileNameListLocal << QFileInfo(filePath).fileName();
        }
        filePathListLocal.sort();
        QList<QPixmap> pixmapListLocal(filePathListLocal.size());

        QModelIndex topIndex = view->indexAt(QPoint(0, 0));
        QModelIndex bottomIndex = view->indexAt(QPoint(0, view->viewport()->height() - 1));

        QMetaObject::invokeMethod(this, [=]() {
            if (!self) {
                return; // If object in main thread does not exist anymore
            }
            this->filePathList = filePathListLocal;
            this->fileNameList = fileNameListLocal;
            this->pixmapList = pixmapListLocal;
            this->model->updateData(fileNameListLocal, pixmapListLocal);
            this->view->viewport()->update();
        }, Qt::QueuedConnection);

        for (int i = 0; i < filePathListLocal.length(); i++) {
            // First load scaled image with fast transformation
            // Resolution should be a bit better than one that will be finally used
            QImageReader reader(filePathListLocal[i]);
            QSizeF size = reader.size();
            qreal imageScale = size.height() / size.width();

            qreal maxOrigSize = std::max(size.height(), size.width());
            qreal resizeOrigFactor = maxOrigSize / (PIXMAP_MAX_SIDE_SIZE * 2);
            if (resizeOrigFactor < 1) { resizeOrigFactor = 1; }

            reader.setScaledSize(QSize(size.width() / resizeOrigFactor, size.height() / resizeOrigFactor));  // Load directly as 64×64 image
            QImage lowResImage = reader.read();
            QPixmap pixmap = QPixmap::fromImage(lowResImage);
            if (!pixmap) {
                pixmap = brokenPixmap;
            }

            // Rescale image with smooth transform
            qreal maxSize = std::max(pixmap.height(), pixmap.width());
            qreal resizeFactor = maxSize / PIXMAP_MAX_SIDE_SIZE;
            pixmapListLocal[i] = pixmap.scaled(QSize(pixmap.width() / resizeFactor, pixmap.height() / resizeFactor),
                                               Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QMetaObject::invokeMethod(this, [=]() {
                if (!self) {
                    return; // If object in main thread does not exist anymore
                }
                this->updateItem(i, filePathListLocal[i], pixmapListLocal[i]);
            }, Qt::QueuedConnection);
        }

        QMetaObject::invokeMethod(this, [=]() {
            if (!self) {
                return; // If object in main thread does not exist anymore
            }
            this->updateView();
        }, Qt::QueuedConnection);
    });
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
