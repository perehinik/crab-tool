#include "file_manager_widget.h"
#include "image_button_widget.h"
#include "file_manager_dir_widget.h"

FileManager::FileManager(QWidget *parent) : QWidget(parent) {
    // setAttribute(Qt::WA_StyledBackground);
    // setObjectName("customFileManager");
    // setStyleSheet(
    //     "#customFileManager { border: 2px solid green; }"
    //     "#customFileManager:hover { border: 2px solid blue; }"
    //     "#customFileManager:pressed { border: 2px solid red; }"
    //     );

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(180);

    containerWidget = new QWidget(this);
    containerLayout = new QGridLayout(containerWidget);
    containerLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);

    updateItems();

    containerWidget->setLayout(containerLayout);
    scrollArea = new QScrollArea();
    scrollArea->setAttribute(Qt::WA_OpaquePaintEvent);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // scrollArea->setStyleSheet("border: 2px solid red;");
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void FileManager::updateItems() {
    // fileList.resize(9);
    fileList = {
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-01-07/capture_00213.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00015.jpg",
        "/home/ivan/proj/TrainingData/RAW/2025-04-28_15-59-36/capture_00039.jpg",
    };
    fileList.sort();
    buttonList.resize(fileList.length());
    for (int i=0; i < fileList.length(); i++) {
        buttonList[i] = new ImgButtonWidget(fileList[i], containerWidget);
        containerLayout->addWidget(buttonList[i], i, 0);
        QObject::connect(buttonList[i], &ImgButtonWidget::onImageButtonClicked, this, &FileManager::onButtonPressed);
    }
}

void FileManager::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    qDebug() << "ContainerWidth: " << containerWidget->width();
    // containerWidget->resize(scrollArea->viewport()->width(), containerWidget->height());

    for (int i=0; i < buttonList.length(); i++) {
        qreal buttonWidth = buttonList[i]->width();
        qreal buttonHeight = buttonList[i]->height();
        qreal maxSize = std::max(buttonWidth, buttonHeight);
        qreal scalingFactor = width() / maxSize;
        buttonList[i]->resize(buttonWidth / scalingFactor, buttonHeight / scalingFactor);
    }
    // scrollArea->setFixedWidth(width());
    // containerWidget->setFixedWidth(width()-20);
    // containerWidget->adjustSize();
}

void FileManager::onButtonPressed(QString imagePath)
{
    emit onImageButtonClicked(imagePath);
}
