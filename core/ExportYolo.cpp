#include "ExportYolo.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QImageReader>
#include <QIcon>
#include <QImage>

ExportYolo::ExportYolo(ProjectData *data, QWidget *parent)
    : ExportBase(data, parent)
{
    setWindowTitle("Export Haar descriptor");
    QStringList tags = data->allTags();
    QPushButton *selectAllBtn = new QPushButton("Select All", this);
    QPushButton *deselectAllBtn = new QPushButton("Deselect All", this);

    tagList = new QListWidget(this);
    for (int i = 0; i < tags.length(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(tags[i], tagList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    connect(selectAllBtn, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tagList->count(); ++i)
            tagList->item(i)->setCheckState(Qt::Checked);
    });

    connect(deselectAllBtn, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < tagList->count(); ++i)
            tagList->item(i)->setCheckState(Qt::Unchecked);
    });

    layout->addWidget(selectAllBtn, 2, 0, 1, 1);
    layout->addWidget(deselectAllBtn, 2, 1, 1, 1);
    layout->addWidget(tagList, 4, 0, 1, 4);
}

QStringList ExportYolo::selectedTags() const {
    QStringList selected;
    for (int i = 0; i < tagList->count(); ++i) {
        QListWidgetItem *item = tagList->item(i);
        if (item->checkState() == Qt::Checked) {
            selected << item->text();
        }
    }
    return selected;
}

void ExportYolo::clearTagsCount(const QStringList &tags) {
    trainTagsCount.clear();
    verifTagsCount.clear();
    for (int tagId = 0; tagId < tags.length(); ++tagId) {
        QString tag = tags[tagId];
        trainTagsCount[tag] = 0;
        verifTagsCount[tag] = 0;
    }
}

// This function decides if image and objects should be used for verification
// If True -> save image for verification, otherwise for training
bool ExportYolo::useForVerif(const QStringList &tags, const QString &imageFileName) {
    int useVerif = 0;
    int useTrain = 0;

    const QString trainFilePath = QDir(exportPath()).filePath("images/train");
    const QString valFilePath = QDir(exportPath()).filePath("images/val");
    // If file with current hash already exported early there is no reason to copy it again
    if (QFile::exists(QDir(trainFilePath).filePath(imageFileName))) {
        return false;
    } else if (QFile::exists(QDir(valFilePath).filePath(imageFileName))) {
        return true;
    }

    for (int tagId = 0; tagId < tags.length(); ++tagId) {
        QString tag = tags[tagId];
        if (!trainTagsCount.contains(tag)) {
            continue;
        }
        if (trainTagsCount[tag] == 0) {
            return false;
        }
        if (verifTagsCount[tag] == 0) {
            return true;
        }
        qreal verifPerc = verifTagsCount[tag] / (trainTagsCount[tag] + verifTagsCount[tag]);
        if (verifPerc < 0.2) {
            useVerif += 1;
        } else {
            useTrain += 1;
        }
    }
    return useVerif > useTrain;
}

void ExportYolo::updateSavedTagCount(const QStringList &tags, bool isVerif) {
    for (int tagId = 0; tagId < tags.length(); ++tagId) {
        QString tag = tags[tagId];
        if (!trainTagsCount.contains(tag)) {
            continue;
        }
        if (isVerif) {
            verifTagsCount[tag] += 1;
        } else {
            trainTagsCount[tag] += 1;
        }
    }
}

void ExportYolo::exportAndSave() {
    showLog();
    clearLog();
    progress->setValue(0);
    progress->show();
    const QStringList tags = selectedTags();
    clearTagsCount(tags);

    QString imagesRootDir = QDir(exportPath()).filePath("images");
    QString labelsRootDir = QDir(exportPath()).filePath("labels");

    QStringList keys = data->keys();
    for (int i = 0; i < keys.length(); ++i) {
        ImageData *imgData = data->getImageDataById(keys[i]);
        QStringList imgTags = imgData->tags();
        addLogMessage(QString("Export: %1").arg(imgData->imageRelativePath));

        const QString yoloDescriptor = getYoloDescriptor(imgData, tags);
        if (yoloDescriptor.isEmpty()) {
            continue;
        }
        const QString imgExtension = QFileInfo(imgData->imageRelativePath).suffix();
        const QString imageFileName = imgData->hash + "." + imgExtension;
        bool useVerif = useForVerif(imgTags, imageFileName);
        const QString dirName = useVerif ? "val" : "train";
        const QString imagesDirPath = QDir(imagesRootDir).filePath(dirName);
        const QString labelsDirPath = QDir(labelsRootDir).filePath(dirName);
        const QString imagePath = QDir(imagesDirPath).filePath(imageFileName);
        const QString labelPath = QDir(labelsDirPath).filePath(imgData->hash + ".txt");
        QDir().mkpath(imagesDirPath);
        QDir().mkpath(labelsDirPath);

        if (!QFile::exists(imagePath)) {
            QString projectImagePath = QDir(data->projectDir()).absoluteFilePath(imgData->imageRelativePath);
            QFile::copy(projectImagePath, imagePath);
        }
        saveTextToFile(labelPath, yoloDescriptor);
        updateSavedTagCount(imgTags, useVerif);

        progress->setValue((float)i / ((float)keys.length() / 100.0));
    }
    saveYaml("images/train", "images/val", tags);
    progress->setValue(100);
    addLogMessage("\n\nSUCCESS !!!", Qt::green);
}

QString ExportYolo::getYoloDescriptor(ImageData *imgData, const QStringList &tags) {
    QString imagePath = QDir(data->projectDir()).absoluteFilePath(imgData->imageRelativePath);
    qDebug() << imgData->selectionCount() << imagePath;
    if (!QFile::exists(imagePath)) {
        addLogMessage("File doesn't exist: " + imagePath, Qt::red);
        return "";
    }
    QSizeF imageSize = QImageReader(imagePath).size();
    QString yoloDescriptor = "";

    for (int selId = 0; selId < imgData->selectionCount(); selId++) {
        QRectF rect = imgData->selectionList[selId]->getRect();
        qreal width = rect.width() / imageSize.width();
        qreal height = rect.height() / imageSize.height();
        qreal centerX = rect.center().x() / imageSize.width();
        qreal centerY = rect.center().y() / imageSize.height();

        QStringList rectTags = imgData->selectionList[selId]->tags;
        for (int i = 0; i < rectTags.length(); ++i) {
            QString tag = rectTags[i];
            if (tags.contains(tag)) {
                int tagId = tags.indexOf(tag);
                yoloDescriptor += QString("%1 %2 %3 %4 %5\n").arg(tagId).arg(centerX).arg(centerY).arg(width).arg(height);
            }
        }
    }
    return yoloDescriptor;
}

void ExportYolo::saveYaml(const QString &trainPath, const QString &valPath, const QStringList &tags) {
    QString yamlString = QString("train: %1\n").arg(trainPath);
    yamlString += QString("val: %1\n\n").arg(valPath);
    yamlString += QString("nc: %1\n").arg(tags.length());
    yamlString += QString("names: ['%1']\n").arg(tags.join("', '"));

    QString yamlFilePath = QDir(exportPath()).filePath("data.yaml");
    saveTextToFile(yamlFilePath, yamlString);
}
