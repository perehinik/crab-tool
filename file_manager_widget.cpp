#include "file_manager_widget.h"
#include "file_manager_dir_widget.h"
#include <QDirIterator>

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

    updateItems();

    QLayout *mainLayout = new QGridLayout();
    setLayout(mainLayout);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void FileManager::updateItems() {
    int i = 0;
    QDirIterator it("/home/ivan/proj/TrainingData/RAW/", QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString subdirPath = it.next();
        FileManagerDir *fileManagerDir = new FileManagerDir(subdirPath, containerWidget);
        containerLayout->addWidget(fileManagerDir, i, 0);
        QObject::connect(fileManagerDir, &FileManagerDir::onImageButtonClicked, this, &FileManager::onButtonPressed);
        buttonList.append(fileManagerDir);
        i++;
    }
}

void FileManager::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void FileManager::onButtonPressed(QString imagePath) {
    emit onImageButtonClicked(imagePath);
}
