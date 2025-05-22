#include "file_manager_dir_widget.h"
#include "image_button_widget.h"
#include <QPushButton>
#include <QDirIterator>

FileManagerDir::FileManagerDir(QString dirPath, QWidget *parent) : QWidget(parent) {
    this->dirPath = dirPath;
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

    QLabel *dirPathLabel = new QLabel(dirPath, controlWidget);
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
    int i = 0;
    QDirIterator it(dirPath, QStringList() << "*.jpg" << "*.png", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        ImgButtonWidget *btn = new ImgButtonWidget(filePath, containerWidget);
        buttonList.append(btn);
        containerLayout->addWidget(btn, i, 0);
        QObject::connect(btn, &ImgButtonWidget::onImageButtonClicked, this, &FileManagerDir::onButtonPressed);
        i++;
        if (i >= 100) {break;}
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
