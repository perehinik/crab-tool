#ifndef MULTIVALUEWIDGET_H
#define MULTIVALUEWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QListWidget>

#include "FlowLayout.h"

#define MULTI_VALUE_TAG_HEIGHT 22
#define MULTI_VALUE_HOR_SPACE 4
#define MULTI_VALUE_MIN_HEIGHT (2 * MULTI_VALUE_HOR_SPACE + MULTI_VALUE_TAG_HEIGHT)

class MultiValueWidget : public QWidget {
    Q_OBJECT
public:
    explicit MultiValueWidget(QWidget *parent = nullptr);

    QStringList values() const;
    QMap<QString, int> allValues;
    void setValues(QStringList valList);
    void addValue(const QString &value, bool updateValueCount = true);
    QSize sizeHint() const override;
    void clear();
    QJsonObject toJson();
    void fromJson(QJsonObject &json);

signals:
    void onValuesChanged();

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
