#ifndef FILE_MANAGER_DIR_WIDGET_H
#define FILE_MANAGER_DIR_WIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QPushButton>
#include "image_button_widget.h"

class FileManagerDir : public QWidget {
    Q_OBJECT

private:
    QString dirPath;
    QGridLayout *mainLayout;
    QWidget *controlWidget;
    QGridLayout *controlLayout;
    QPushButton *minimiseButton;
    QWidget *containerWidget;
    QGridLayout *containerLayout;
    QList<QString> fileList;
    QList<ImgButtonWidget*> buttonList;
    void onButtonPressed(QString imagePath);
    void onFolderClick();

public:
    explicit FileManagerDir(QString dirPath, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void updateItems();

signals:
    void onImageButtonClicked(QString imagePath);
};

#endif // FILE_MANAGER_DIR_WIDGET_H
