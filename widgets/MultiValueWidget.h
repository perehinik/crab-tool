#ifndef MULTIVALUEWIDGET_H
#define MULTIVALUEWIDGET_H

#include <QWidget>
#include <QStringList>

class QToolButton;
class QLineEdit;
class FlowLayout;
class QFrame;
class QListWidget;
class QListWidgetItem;

class MultiValueWidget : public QWidget {
    Q_OBJECT
public:
    explicit MultiValueWidget(QWidget *parent = nullptr);

    QStringList values() const;
    void addValue(const QString &value);
    QSize sizeHint() const override;
    void clear();

signals:
    void valuesChanged();

private slots:
    void showPopup();
    void handleAddNew();
    void handleListItemClicked(QListWidgetItem *item);

private:
    FlowLayout *tagLayout;

    QFrame *popup;
    QListWidget *valueList;
    QLineEdit *newEntry;
    QIcon deleteIcon;

    void createTag(const QString &text);
    void removeTag(QWidget *tagWidget);
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // MULTIVALUEWIDGET_H
