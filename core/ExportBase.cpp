#include "ExportBase.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QIcon>

ExportBase::ExportBase(ProjectData *data, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Export");
    this->data = data;

    QGridLayout *layout = new QGridLayout(this);

    // Directory input and browse button
    pathEdit = new QLineEdit(this);
    QPushButton *browseBtn = new QPushButton(this);
    browseBtn->setIcon(QIcon(":/icon/folder-fill.png"));
    connect(browseBtn, &QPushButton::clicked, this, &ExportBase::openDirectoryDialog);

    QPushButton *selectAllBtn = new QPushButton("Select All", this);
    QPushButton *deselectAllBtn = new QPushButton("Deselect All", this);

    tagList = new QListWidget(this);

    QStringList tags = data->allTags();
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

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &ExportBase::acceptExportHandler);
    connect(buttons, &QDialogButtonBox::rejected, this, &ExportBase::rejectExportHandler);

    layout->addWidget(pathEdit, 0, 0, 1, 4);
    layout->addWidget(browseBtn, 0, 4, 1, 1);
    layout->addWidget(selectAllBtn, 1, 0, 1, 1);
    layout->addWidget(deselectAllBtn, 1, 1, 1, 1);
    layout->addWidget(tagList, 2, 0, 1, 5);
    layout->addWidget(buttons, 4, 2, 1, 2);
}

void ExportBase::acceptExportHandler() {
    this->accept();
}

void ExportBase::rejectExportHandler() {
    this->reject();
}

void ExportBase::openDirectoryDialog() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Export Directory");
    if (!dir.isEmpty()) {
        pathEdit->setText(dir);
    }
}

QString ExportBase::exportPath() const {
    return pathEdit->text();
}

QStringList ExportBase::selectedTags() const {
    QStringList selected;
    for (int i = 0; i < tagList->count(); ++i) {
        QListWidgetItem *item = tagList->item(i);
        if (item->checkState() == Qt::Checked) {
            selected << item->text();
        }
    }
    return selected;
}
