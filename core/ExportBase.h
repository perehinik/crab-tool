#ifndef EXPORTBASE_H
#define EXPORTBASE_H

#include "ProjectData.h"

#include <QDialog>
#include <QStringList>
#include <QDialogButtonBox>
#include <QLabel>

class QLineEdit;
class QPushButton;
class QListWidget;

class ExportBase : public QDialog {
    Q_OBJECT

public:
    ExportBase(ProjectData *data, QWidget *parent = nullptr);

    QString exportPath() const;
    QStringList selectedTags() const;

private slots:
    void openDirectoryDialog();

private:
    ProjectData * data;
    QDialogButtonBox *buttons;
    QLineEdit *pathEdit;
    QLabel *errorLabel;
    QListWidget *tagList;

    void acceptExportHandler();
    void rejectExportHandler();
    void showError(QString error);

    virtual void exportAndSave();
};

#endif // EXPORTBASE_H
