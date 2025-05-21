#ifndef FILE_MANAGER_DIR_WIDGET_H
#define FILE_MANAGER_DIR_WIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include "image_button_widget.h"

class FileManagerDir : public QWidget {
    Q_OBJECT

private:
    QWidget *mainWidget;
    QGridLayout *mainLayout;
    QWidget *containerWidget;
    QGridLayout *containerLayout;
    QList<QString> fileList;
    QList<ImgButtonWidget*> buttonList;
    void onButtonPressed(QString imagePath);

public:
    explicit FileManagerDir(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void updateItems();

signals:
    void onImageButtonClicked(QString imagePath);
};

#endif // FILE_MANAGER_DIR_WIDGET_H
