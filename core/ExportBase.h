#ifndef EXPORTBASE_H
#define EXPORTBASE_H

#include "ProjectData.h"

#include <QDialog>
#include <QGridLayout>
#include <QStringList>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>
#include <QTextEdit>

class QLineEdit;
class QPushButton;
class QListWidget;

class ExportBase : public QDialog {
    Q_OBJECT

public:
    ExportBase(ProjectData *data, QWidget *parent = nullptr);
    QString exportPath() const;
    void deleteFilesIfExist(QString dirPath, QString fileName);
    void addLogMessage(const QString &message, const QColor &color = Qt::black);
    void hideLog();
    void showLog();

    QGridLayout *layout;
    ProjectData *data;
    QProgressBar *progress;

private slots:
    void openDirectoryDialog();

private:
    QDialogButtonBox *buttons;
    QLineEdit *pathEdit;
    QLabel *statusLabel;
    QTimer *statusHideTimer;
    QTextEdit *logView;

    void acceptExportHandler();
    void rejectExportHandler();
    void showStatus(QString error, QColor color = Qt::black, bool clear = false);
    void hideStatus();

    virtual void exportAndSave();
};

#endif // EXPORTBASE_H
