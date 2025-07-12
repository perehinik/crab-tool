#include "ExportBase.h"

#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>

ExportBase::ExportBase(ProjectData *data, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Export");
    this->data = data;

    layout = new QGridLayout(this);

    QWidget *pathWidget = new QWidget(this);
    QHBoxLayout *pathWidgetLayout = new QHBoxLayout(pathWidget);
    pathWidgetLayout->setContentsMargins(0, 0, 0, 0);
    pathWidgetLayout->setSpacing(6);

    // Directory input and browse button
    pathEdit = new QLineEdit(this);
    pathEdit->setText(data->projectDir());

    QPushButton *browseBtn = new QPushButton(this);
    browseBtn->setIcon(QIcon(":/icon/folder-fill.png"));
    connect(browseBtn, &QPushButton::clicked, this, &ExportBase::openDirectoryDialog);

    pathWidgetLayout->addWidget(pathEdit);
    pathWidgetLayout->addWidget(browseBtn);

    statusLabel = new QLabel(this);
    statusLabel->hide();
    statusLabel->setAutoFillBackground(false);
    // errorLabel->setStyleSheet("color: red; font-size: 11px;");

    statusHideTimer = new QTimer(this);
    statusHideTimer->setSingleShot(true);
    connect(statusHideTimer, &QTimer::timeout, this, [this]() {
        statusLabel->hide();
    });

    progress = new QProgressBar(this);
    progress->setRange(0, 100);
    progress->setValue(0);
    progress->hide();

    logView = new QTextEdit(this);
    logView->setReadOnly(true);
    logView->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);  // allow copy, disable edit
    logView->setLineWrapMode(QTextEdit::NoWrap);
    logView->setMaximumHeight(100);
    logView->hide();

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &ExportBase::acceptExportHandler);
    connect(buttons, &QDialogButtonBox::rejected, this, &ExportBase::rejectExportHandler);

    layout->addWidget(pathWidget, 0, 0, 1, 4);
    layout->addWidget(logView, 18, 0, 1, 4);
    layout->addWidget(statusLabel, 20, 0, 1, 4);
    layout->addWidget(progress, 22, 0, 1, 4);
    layout->addWidget(buttons, 24, 2, 1, 2);
}

void ExportBase::exportAndSave() {
    QMessageBox::information(this, "Notice", "Please implement /void exportAndSave()/.", QMessageBox::Cancel);
}

void ExportBase::showLog() {
    logView->show();
}

void ExportBase::hideLog() {
    logView->hide();
}

void ExportBase::addLogMessage(const QString &message, const QColor &color) {
    QTextCharFormat fmt;
    fmt.setForeground(color);

    QTextCursor cursor = logView->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(message + "\n", fmt);
    logView->setTextCursor(cursor);

    const int maxLines = 10000;
    if (logView->document()->blockCount() > maxLines) {
        QTextCursor cur(logView->document());
        cur.movePosition(QTextCursor::Start);
        for (int i = 0; i < logView->document()->blockCount() - maxLines; ++i) {
            cur.select(QTextCursor::BlockUnderCursor);
            cur.removeSelectedText();
            cur.deleteChar();
        }
    }
}

void ExportBase::hideStatus() {
    statusHideTimer->stop();
    statusLabel->hide();
}

void ExportBase::showStatus(QString error, QColor color, bool clear) {
    statusHideTimer->stop();
    QPalette palette = statusLabel->palette();
    palette.setColor(QPalette::WindowText, color);
    statusLabel->setPalette(palette);
    statusLabel->setText(error);
    statusLabel->show();
    if (clear) {
        statusHideTimer->start(2000);
    }
}

void ExportBase::acceptExportHandler() {
    if (QDir(pathEdit->text()).exists()) {
        exportAndSave();
        // this->accept();
    } else {
        showStatus("Directory doesn't exist !", Qt::red, true);
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
        addLogMessage("Selected export directory: " + dir);
    }
}

void ExportBase::deleteFilesIfExist(QString dirPath, QString fileName) {
    QDir dir(dirPath);
    dir.setNameFilters({ fileName });
    dir.setFilter(QDir::Files);

    const QStringList files = dir.entryList();
    for (int fileId = 0; fileId < files.length(); ++fileId) {
        if (!QFile::remove(dir.filePath(files[fileId]))) {
            addLogMessage("Failed to delete: " + files[fileId], Qt::yellow);
        } else {
            addLogMessage("Deleted: " + files[fileId]);
        }
    }
}

QString ExportBase::exportPath() const {
    return pathEdit->text();
}
