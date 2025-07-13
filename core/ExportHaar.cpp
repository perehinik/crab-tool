#include "ExportHaar.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QImageReader>
#include <QIcon>
#include <QImage>

ExportHaar::ExportHaar(ProjectData *data, QWidget *parent)
    : ExportBase(data, parent)
{
    setWindowTitle("Export Haar descriptor");
    QStringList tags = data->allTags();

    exportThreadPool = new QThreadPool(this);

    // Positives
    QPushButton *selectAllBtnPositives = new QPushButton("Select All", this);
    QPushButton *deselectAllBtnPositives = new QPushButton("Deselect All", this);

    tagListPositives = new QListWidget(this);
    for (int i = 0; i < tags.length(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(tags[i], tagListPositives);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    connect(selectAllBtnPositives, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tagListPositives->count(); ++i)
            tagListPositives->item(i)->setCheckState(Qt::Checked);
    });

    connect(deselectAllBtnPositives, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tagListPositives->count(); ++i)
            tagListPositives->item(i)->setCheckState(Qt::Unchecked);
    });

    // Negatives
    QPushButton *selectAllBtnNegatives = new QPushButton("Select All", this);
    QPushButton *deselectAllBtnNegatives = new QPushButton("Deselect All", this);

    tagListNegatives = new QListWidget(this);
    for (int i = 0; i < tags.length(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(tags[i], tagListNegatives);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    connect(selectAllBtnNegatives, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tagListNegatives->count(); ++i)
            tagListNegatives->item(i)->setCheckState(Qt::Checked);
    });

    connect(deselectAllBtnNegatives, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tagListNegatives->count(); ++i)
            tagListNegatives->item(i)->setCheckState(Qt::Unchecked);
    });

    layout->addWidget(selectAllBtnPositives, 2, 0, 1, 1);
    layout->addWidget(deselectAllBtnPositives, 2, 1, 1, 1);
    layout->addWidget(selectAllBtnNegatives, 2, 2, 1, 1);
    layout->addWidget(deselectAllBtnNegatives, 2, 3, 1, 1);
    layout->addWidget(tagListPositives, 4, 0, 1, 2);
    layout->addWidget(tagListNegatives, 4, 2, 1, 2);
}

QStringList ExportHaar::selectedTagsPositive() const {
    QStringList selected;
    for (int i = 0; i < tagListPositives->count(); ++i) {
        QListWidgetItem *item = tagListPositives->item(i);
        if (item->checkState() == Qt::Checked) {
            selected << item->text();
        }
    }
    return selected;
}

QStringList ExportHaar::selectedTagsNegative() const {
    QStringList selected;
    for (int i = 0; i < tagListNegatives->count(); ++i) {
        QListWidgetItem *item = tagListNegatives->item(i);
        if (item->checkState() == Qt::Checked) {
            selected << item->text();
        }
    }
    return selected;
}

void ExportHaar::exportAndSave() {
    showLog();
    clearLog();
    progress->setValue(0);
    progress->show();
    const QStringList positives = selectedTagsPositive();
    const QStringList negatives = selectedTagsNegative();

    for (int i = 0; i < positives.length(); ++i) {
        deleteFilesIfExist(exportPath(), QString("haar*-%1.txt").arg(positives[i]));
    }
    for (int i = 0; i < negatives.length(); ++i) {
        deleteFilesIfExist(exportPath(), QString("haar*-%1.txt").arg(negatives[i]));
    }

    QStringList keys = data->keys();
    for (int i = 0; i < keys.length(); ++i) {
        ImageData *imgData = data->getImageDataById(keys[i]);
        addLogMessage(QString("Export: %1").arg(imgData->imageRelativePath));

        exportImageData(imgData, positives, negatives);
        progress->setValue((float)i / ((float)keys.length() / 100.0));
    }
    exportThreadPool->waitForDone();
    progress->setValue(100);
    addLogMessage("\n\nSUCCESS !!!", Qt::green);
}

void ExportHaar::exportImageData(ImageData *imgData, const QStringList &positives, const QStringList &negatives) {
    QList<SelectionRect*> selList;
    const QStringList tags = imgData->tags();

    for (int i = 0; i < tags.length(); i++) {
        bool savePos = positives.contains(tags[i]);
        bool saveNeg = negatives.contains(tags[i]);
        if (!saveNeg && !savePos) {
            continue;
        }
        imgData->getSelectionsByTag(tags[i], selList);
        if (!selList.isEmpty() && savePos) {
            savePositive(tags[i], imgData->imageRelativePath, selList);
        }
        if (!selList.isEmpty() && saveNeg) {
            saveNegative(tags[i], imgData->imageRelativePath, selList);
        }
    }
}

void ExportHaar::savePositive(QString tag, QString imgPath, QList<SelectionRect*> &selList) {
    QString imagePath = QDir(data->projectDir()).absoluteFilePath(imgPath);
    QString haarLine = QDir(exportPath()).relativeFilePath(imagePath);
    haarLine += QString("  %1").arg(selList.length());
    for (int selId = 0; selId < selList.length(); ++selId) {
        QRectF rect = selList[selId]->getRect();
        haarLine += QString("  %1").arg(rect.x());
        haarLine += QString(" %1").arg(rect.y());
        haarLine += QString(" %1").arg(rect.width());
        haarLine += QString(" %1").arg(rect.height());
    }
    QString haarFile = QDir(exportPath()).filePath(QString("haar-%1.txt").arg(tag));

    appendTextToFile(haarFile, haarLine + "\n");
}

void ExportHaar::saveNegative(QString tag, QString imgPath, QList<SelectionRect*> &selList) {
    QString imagePath = QDir(data->projectDir()).absoluteFilePath(imgPath);
    if (!QFile(imagePath).exists()) {
        addLogMessage("File doesn't exist: " + imagePath, Qt::red);
        return;
    }
    QList<QRect> rectList;
    QString haarFile = QDir(exportPath()).filePath(QString("haar-n-%1.txt").arg(tag));

    for (int selId = 0; selId < selList.length(); ++selId) {
        QRectF cropRect = selList[selId]->getRect();
        QSize imageSize = QImageReader(imagePath).size();
        const int w_diff = std::abs(cropRect.width() - imageSize.width());
        const int h_diff = std::abs(cropRect.height() - imageSize.height());

        // If there is at least 1 selection that selects more than 99% of image -> just use the whole image.
        // In most cases it should be done like that - for better efficiency.
        if (w_diff <= 0.01 * (float)imageSize.width() && h_diff <= 0.01 * (float)imageSize.height()) {
            QString haarLines = QDir(exportPath()).relativeFilePath(imagePath) + "\n";
            appendTextToFile(haarFile, haarLines);
            rectList.clear();
            break;
        }
        rectList.append(cropRect.toRect());
    }

    // In case we actually need to crop image - do it in separate thread
    if (!rectList.empty()) {
        while (exportThreadPool->activeThreadCount() >= 16) {
            // For proper process bar visualization
            QThread::msleep(50);
        }
        exportThreadPool->start([=]() {
            QFileInfo imgFileileInfo(imgPath);
            QString negativeSavePath = QDir(exportPath()).filePath("negative");
            QString copyPath = QDir(negativeSavePath).absoluteFilePath(imgFileileInfo.path());
            QImage original(imagePath);
            QString haarLinesLocal = "";

            QDir().mkpath(copyPath);

            for (int rectId = 0; rectId < rectList.length(); ++rectId) {
                QString copyFile = QDir(copyPath).filePath(QString("%1-%2-%3").arg(tag).arg(rectId).arg(imgFileileInfo.fileName()));

                if (!original.isNull() && rectList[rectId].isValid() && original.rect().contains(rectList[rectId])) {
                    QImage cropped = original.copy(rectList[rectId]);
                    cropped.save(copyFile);
                    haarLinesLocal += QDir(exportPath()).relativeFilePath(copyFile) + "\n";
                } else {
                    addLogMessage("Invalid crop area or image " + tag + " " + imagePath, Qt::yellow);
                }
            }
            // Make sure noone else tries to write to same file.
            QMutexLocker locker(&fileWriteMutex);
            appendTextToFile(haarFile, haarLinesLocal);
        });
    }
}
