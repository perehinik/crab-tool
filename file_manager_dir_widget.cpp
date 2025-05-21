#include "file_manager_dir_widget.h"
#include "image_button_widget.h"
#include <QPushButton>

FileManagerDir::FileManagerDir(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(180);

    mainWidget = new QWidget(this);
    mainLayout = new QGridLayout(mainWidget);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);
    mainWidget->setLayout(mainLayout);
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget *controlWidget = new QWidget(mainWidget);
    QGridLayout *controlLayout = new QGridLayout(controlWidget);
    controlWidget->setLayout(controlLayout);

    QPushButton *minimiseButton = new QPushButton();
    minimiseButton->setIcon(QIcon("/home/ivan/proj/CrabTool/icon/folder-open-fill.png"));
    minimiseButton->setIconSize(QSize(32, 32)); // Set desired icon size
    minimiseButton->setFixedSize(32, 32);       // Match button size to icon

    // Remove text, padding, and margins
    minimiseButton->setText("");
    minimiseButton->setStyleSheet(
        "QPushButton {"
        "  border: none;"
        "  padding: 0px;"
        "  margin: 0px;"
        "  background: transparent;"
        "}"
        );

    QLabel *dirPathLabel = new QLabel("/path/to/dir", controlWidget);

    controlLayout->addWidget(minimiseButton);
    controlLayout->addWidget(dirPathLabel);

    containerWidget = new QWidget(this);
    containerLayout = new QGridLayout(containerWidget);
    containerLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);
    containerWidget->setLayout(containerLayout);

    updateItems();

    mainLayout->addWidget(controlWidget, 0, 0);
    mainLayout->addWidget(controlWidget, 1, 0);
}

void FileManagerDir::updateItems() {
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
        QObject::connect(buttonList[i], &ImgButtonWidget::onImageButtonClicked, this, &FileManagerDir::onButtonPressed);
    }
}

void FileManagerDir::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    for (int i=0; i < buttonList.length(); i++) {
        qreal buttonWidth = buttonList[i]->width();
        qreal buttonHeight = buttonList[i]->height();
        qreal maxSize = std::max(buttonWidth, buttonHeight);
        qreal scalingFactor = width() / maxSize;
        buttonList[i]->resize(buttonWidth / scalingFactor, buttonHeight / scalingFactor);
    }
}

void FileManagerDir::onButtonPressed(QString imagePath)
{
    emit onImageButtonClicked(imagePath);
}
