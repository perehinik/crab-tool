#include "ExportBase.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QIcon>
#include <QTimer>
#include <QMessageBox>

ExportBase::ExportBase(ProjectData *data, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Export");
    this->data = data;

    QGridLayout *layout = new QGridLayout(this);

    // Directory input and browse button
    pathEdit = new QLineEdit(this);
    pathEdit->setText(data->projectDir());

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

    errorLabel = new QLabel(this);
    errorLabel->hide();
    errorLabel->setStyleSheet("color: red; font-size: 11px;");

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &ExportBase::acceptExportHandler);
    connect(buttons, &QDialogButtonBox::rejected, this, &ExportBase::rejectExportHandler);

    layout->addWidget(pathEdit, 0, 0, 1, 4);
    layout->addWidget(browseBtn, 0, 4, 1, 1);
    layout->addWidget(selectAllBtn, 2, 0, 1, 1);
    layout->addWidget(deselectAllBtn, 2, 1, 1, 1);
    layout->addWidget(tagList, 4, 0, 1, 5);
    layout->addWidget(errorLabel, 6, 0, 1, 5);
    layout->addWidget(buttons, 8, 2, 1, 2);
}

void ExportBase::exportAndSave() {
    QMessageBox::information(this, "Notice", "Please implement /void exportAndSave()/.", QMessageBox::Cancel);
}

void ExportBase::showError(QString error) {
    errorLabel->setText(error);
    errorLabel->show();
    QTimer::singleShot(2000, this, [this]() {
        errorLabel->hide();
    });
}

void ExportBase::acceptExportHandler() {
    if (QDir(pathEdit->text()).exists()) {
        exportAndSave();
        this->accept();
    } else {
        showError("Directory doesn't exist !");
    }
}

void ExportBase::rejectExportHandler() {
    this->reject();
}

void ExportBase::openDirectoryDialog() {
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Select Export Directory",
        QDir(pathEdit->text()).exists() ? pathEdit->text(): data->projectDir()
    );
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
