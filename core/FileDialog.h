#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QLayout>
#include <QStyle>

QStringList openImagesDialog(QWidget *parent, QString dirPath);
QString openProjectDialog(QWidget *parent, QString dirPath);
QString createProjectDialog(QWidget *parent, QString dirPath);
QString saveProjectDialog(QWidget *parent, QString dirPath);
QString openDirDialog(QWidget *parent, QString dirPath);

#endif // FILEDIALOG_H
