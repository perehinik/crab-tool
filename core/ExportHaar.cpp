#include "ExportHaar.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QIcon>

ExportHaar::ExportHaar(ProjectData *data, QWidget *parent)
    : ExportBase(data, parent)
{
    setWindowTitle("Export Haar descriptor");
}

void ExportHaar::acceptExportHandler() {
    this->accept();
}

void ExportHaar::rejectExportHandler() {
    this->reject();
}
