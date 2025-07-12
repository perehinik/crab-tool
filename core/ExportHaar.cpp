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
        exportPositives(imgData, positives);
        exportNegatives(imgData, negatives);
        progress->setValue((float)i / ((float)keys.length() / 100.0));
    }
    progress->setValue(100);
    addLogMessage("\n\nSUCCESS !!!", Qt::green);
}

void ExportHaar::exportPositives(ImageData *imgData, const QStringList &positives) {
    QList<SelectionRect*> selList;
    const QStringList tags = imgData->tags();
    for (int i = 0; i < tags.length(); i++) {
        if (!positives.contains(tags[i])) {
            continue;
        }
        imgData->getSelectionsByTag(tags[i], selList);
        if (!selList.isEmpty()) {
            savePositive(tags[i], imgData->imageRelativePath, selList);
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

    QFile file(haarFile);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << haarLine << "\n";
        file.close();
    } else {
        addLogMessage("Cannot open file for writing: " + haarFile, Qt::red);
        addLogMessage(file.errorString(), Qt::red);
    }
}

void ExportHaar::exportNegatives(ImageData *imgData, const QStringList &negatives) {
    QList<SelectionRect*> selList;
    const QStringList tags = imgData->tags();
    for (int i = 0; i < tags.length(); i++) {
        if (!negatives.contains(tags[i])) {
            continue;
        }
        imgData->getSelectionsByTag(tags[i], selList);
        if (!selList.isEmpty()) {
            saveNegative(tags[i], imgData->imageRelativePath, selList);
        }
    }
}

void ExportHaar::saveNegative(QString tag, QString imgPath, QList<SelectionRect*> &selList) {
    QString imagePath = QDir(data->projectDir()).absoluteFilePath(imgPath);
    if (!QFile(imagePath).exists()) {
        addLogMessage("File doesn't exist: " + imagePath, Qt::red);
        return;
    }
    QString haarLines = "";
    QList<QRect> rectList;

    for (int selId = 0; selId < selList.length(); ++selId) {
        QRectF cropRect = selList[selId]->getRect();
        QSize imageSize = QImageReader(imagePath).size();
        const int w_diff = std::abs(cropRect.width() - imageSize.width());
        const int h_diff = std::abs(cropRect.height() - imageSize.height());

        // If there is at least 1 selection that selects more than 99% of image -> just use the whole image.
        // In most cases it should be done like that - for better efficiency.
        if (w_diff <= 0.01 * (float)imageSize.width() && h_diff <= 0.01 * (float)imageSize.height()) {
            haarLines = QDir(exportPath()).relativeFilePath(imagePath) + "\n";
            rectList.clear();
            break;
        }
        rectList.append(cropRect.toRect());
    }

    if (!rectList.empty()) {
        QFileInfo imgFileileInfo(imgPath);
        QString negativeSavePath = QDir(exportPath()).filePath("negative");
        QString copyPath = QDir(negativeSavePath).absoluteFilePath(imgFileileInfo.path());
        QImage original(imagePath);

        QDir().mkpath(copyPath);

        for (int rectId = 0; rectId < rectList.length(); ++rectId) {
            QString copyFile = QDir(copyPath).filePath(QString("%1-%2-%3").arg(tag).arg(rectId).arg(imgFileileInfo.fileName()));

            if (!original.isNull() && rectList[rectId].isValid() && original.rect().contains(rectList[rectId])) {
                QImage cropped = original.copy(rectList[rectId]);
                cropped.save(copyFile);
                haarLines += QDir(exportPath()).relativeFilePath(copyFile) + "\n";
            } else {
                addLogMessage("Invalid crop area or image " + tag + " " + imagePath, Qt::yellow);
            }
        }
    }

    QString haarFile = QDir(exportPath()).filePath(QString("haar-n-%1.txt").arg(tag));

    QFile file(haarFile);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << haarLines;
        file.close();
    } else {
        addLogMessage("Cannot open file for writing: " + haarFile, Qt::red);
        addLogMessage(file.errorString(), Qt::red);
    }
}
