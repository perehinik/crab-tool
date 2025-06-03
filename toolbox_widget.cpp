#include "toolbox_widget.h"
#include <QPalette>
#include <QGridLayout>
#include <QToolButton>
#include <QPushButton>
#include <QFileDialog>
#include "ToolboxButton.h"

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

    layout->addWidget(openFileButton, 0, 0);
    layout->addWidget(openDirButton, 0, 1);
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
}
