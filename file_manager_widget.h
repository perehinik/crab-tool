#ifndef FILE_MANAGER_WIDGET_H
#define FILE_MANAGER_WIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>

class FileManager : public QWidget {
    Q_OBJECT

private:
    QScrollArea *scrollArea;
    QWidget *containerWidget;
    QGridLayout *containerLayout;
    QList<QString> fileList;
    QList<QWidget*> buttonList;
    void onButtonPressed(QString imagePath);

public:
    explicit FileManager(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void updateItems(QString dirPath);

signals:
    void onImageButtonClicked(QString imagePath);
};

#endif // FILE_MANAGER_WIDGET_H
