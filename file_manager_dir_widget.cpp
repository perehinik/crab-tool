#include "file_manager_dir_widget.h"
#include "image_button_widget.h"
#include <QPushButton>

FileManagerDir::FileManagerDir(QWidget *parent) : QWidget(parent) {
    mainLayout = new QGridLayout();
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    controlWidget = new QWidget(this);
    controlLayout = new QGridLayout();
    controlWidget->setLayout(controlLayout);
    controlWidget->setMinimumSize(QSize(150, 40));
    controlWidget->setStyleSheet(
        "  border: none;"
        "  padding: 0px;"
        "  margin: 0px;"
        "  background: lightgrey;"
        );
    controlWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    minimiseButton = new QPushButton();
    minimiseButton->setIcon(QIcon("/home/ivan/proj/crab-tool/icon/folder-open-fill.png"));
    minimiseButton->setIconSize(QSize(24, 24)); // Set desired icon size
    minimiseButton->setFixedSize(24, 24);       // Match button size to icon
    QObject::connect(minimiseButton, &QPushButton::released, this, &FileManagerDir::onFolderClick);

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

    QLabel *dirPathLabel = new QLabel("/very/verylong/path/to/dir", controlWidget);
    dirPathLabel->setFixedHeight(24);
    dirPathLabel->setWordWrap(true);
    dirPathLabel->setAlignment(Qt::AlignVCenter);

    controlLayout->addWidget(minimiseButton, 0, 0);
    controlLayout->addWidget(dirPathLabel, 0, 1);
    controlLayout->setAlignment(Qt::AlignVCenter);
    controlLayout->setContentsMargins(5, 0, 0, 0);
    controlLayout->setSpacing(5);

    containerWidget = new QWidget(this);
    containerLayout = new QGridLayout(containerWidget);
    containerLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);
    containerWidget->setLayout(containerLayout);

    updateItems();

    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    mainLayout->addWidget(controlWidget, 0, 0);
    mainLayout->addWidget(containerWidget, 1, 0);
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
    for (int i = 0; i < fileList.length(); i++) {
        buttonList[i] = new ImgButtonWidget(fileList[i], containerWidget);
        containerLayout->addWidget(buttonList[i], i, 0);
        QObject::connect(buttonList[i], &ImgButtonWidget::onImageButtonClicked, this, &FileManagerDir::onButtonPressed);
    }
}

void FileManagerDir::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void FileManagerDir::onFolderClick() {
    if (containerWidget->isHidden()) {
        containerWidget->show();
        minimiseButton->setIcon(QIcon("/home/ivan/proj/crab-tool/icon/folder-open-fill.png"));
    } else {
        containerWidget->hide();
        minimiseButton->setIcon(QIcon("/home/ivan/proj/crab-tool/icon/folder-fill.png"));
    }
}

void FileManagerDir::onButtonPressed(QString imagePath) {
    emit onImageButtonClicked(imagePath);
}
