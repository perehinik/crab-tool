#ifndef IMAGE_BUTTON_WIDGET_H
#define IMAGE_BUTTON_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class ImgButtonWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImgButtonWidget(QString imagePath = "", QWidget *parent = nullptr);

private:
    QLabel *backgroundLabel;
    QLabel *nameLabel;
    QPixmap pixmap;
    QString imagePath;
    qreal previousWidth = 0;
    qreal imageScale = 1;
    void loadImage();
    void updateBackground();
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void onImageButtonClicked(QString imagePath);
};

#endif // IMAGE_BUTTON_WIDGET_H
