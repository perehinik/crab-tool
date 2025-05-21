#include "file_manager_widget.h"
#include "file_manager_dir_widget.h"

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
    FileManagerDir *fileManagerDir = new FileManagerDir(containerWidget);
    containerLayout->addWidget(fileManagerDir, 0, 0);
    QObject::connect(fileManagerDir, &FileManagerDir::onImageButtonClicked, this, &FileManager::onButtonPressed);
    buttonList.resize(1);
    buttonList[0] = fileManagerDir;
}

void FileManager::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void FileManager::onButtonPressed(QString imagePath) {
    emit onImageButtonClicked(imagePath);
}
