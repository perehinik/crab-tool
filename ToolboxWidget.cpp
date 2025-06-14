#include "ToolboxWidget.h"
#include <QPalette>
#include <QGridLayout>
#include <QToolButton>
#include <QPushButton>
#include <QFileDialog>
#include <QCursor>
#include "ToolboxButtonWidget.h"

#define TOOLBOX_BUTTON_HEIGHT 30

ToolboxWidget::ToolboxWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("border: 1px solid lightgrey;");
    setFixedHeight(TOOLBOX_BUTTON_HEIGHT);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    layout->setSpacing(0);

    ToolboxButton *openFileButton = new ToolboxButton(QIcon(":/icon/file-add-fill.png"), "Open File", 32, 20, this);
    connect(openFileButton->action, &QAction::triggered, this, &ToolboxWidget::onOpenFile);

    ToolboxButton *openDirButton = new ToolboxButton(QIcon(":/icon/folder-fill.png"), "Open Directory", 32, 24, this);
    connect(openDirButton->action, &QAction::triggered, this, &ToolboxWidget::onOpenDir);

    saveButton = new ToolboxButton(QIcon(":/icon/save.png"), "Save", 32, 24, this);

    handToolButton = new ToolboxButton(QIcon(":/icon/drag.png"), "Move", 32, 24, this);
    handToolButton->action->setCheckable(true);

    zoomInToolButton = new ToolboxButton(QIcon(":/icon/zoom-in.png"), "Zoom In", 32, 24, this);
    zoomOutToolButton = new ToolboxButton(QIcon(":/icon/zoom-out.png"), "Zoom Out", 32, 24, this);
    zoomToExtentsToolButton = new ToolboxButton(QIcon(":/icon/zoom-to-extents.png"), "Zoom To Extents", 32, 24, this);

    layout->addWidget(openFileButton, 0, 0);
    layout->addWidget(openDirButton, 0, 1);
    layout->addWidget(saveButton, 0, 2);
    layout->addWidget(handToolButton, 0, 3);
    layout->addWidget(zoomInToolButton, 0, 4);
    layout->addWidget(zoomOutToolButton, 0, 5);
    layout->addWidget(zoomToExtentsToolButton, 0, 6);
}

void ToolboxWidget::onOpenFile() {
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        "Select Files",
        QDir::homePath(),
        "Images (*.png *.jpg *.bmp);;All Files (*.*)"
    );

    if (fileNames.isEmpty()) {
        return;
    }
    emit onFilesOpen(fileNames);
}

void ToolboxWidget::onOpenDir() {
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "Select Directory",
        QDir::homePath(),  // starting path
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (dirPath.isEmpty()) {
        return;
    }
    emit onDirOpen(dirPath);
}
