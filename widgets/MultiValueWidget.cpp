#include "MultiValueWidget.h"
#include "FlowLayout.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>
#include <QApplication>

MultiValueWidget::MultiValueWidget(QWidget *parent)
    : QWidget(parent)
{
    tagLayout = new FlowLayout(this);
    tagLayout->setContentsMargins( 3, 3, 3, 3);

    deleteIcon = QIcon(":/icon/delete.png");

    popup = new QFrame(nullptr, Qt::Popup);
    popup->setFrameShape(QFrame::Box);
    popup->setMaximumHeight(150);
    QVBoxLayout *popupLayout = new QVBoxLayout(popup);
    popupLayout->setContentsMargins(2, 2, 2, 2);

    valueList = new QListWidget(popup);
    popupLayout->addWidget(valueList);

    newEntry = new QLineEdit(popup);
    newEntry->setPlaceholderText("Enter new value");
    popupLayout->addWidget(newEntry);

    connect(newEntry, &QLineEdit::returnPressed, this, &MultiValueWidget::handleAddNew);
    connect(valueList, &QListWidget::itemClicked, this, &MultiValueWidget::handleListItemClicked);
}

QSize MultiValueWidget::sizeHint() const
{
    return layout()->sizeHint();
}

void MultiValueWidget::showPopup()
{
    QPoint globalBottomLeft = mapToGlobal(QPoint(0, height()));

    popup->resize(width(), newEntry->height());
    valueList->hide();
    popup->move(globalBottomLeft);
    popup->show();
    newEntry->setFocus();
}

void MultiValueWidget::handleAddNew()
{
    QString text = newEntry->text().trimmed();
    if (!text.isEmpty()) {
        addValue(text);
        newEntry->clear();
        popup->hide();
        emit valuesChanged();
    }
}

void MultiValueWidget::handleListItemClicked(QListWidgetItem *item)
{
    if (item) {
        addValue(item->text());
        popup->hide();
        emit valuesChanged();
    }
}

void MultiValueWidget::addValue(const QString &text)
{
    if (values().contains(text)) {
        return;
    }

    QWidget *tag = new QWidget(this);
    tag->setProperty("tagText", text);

    QHBoxLayout *layout = new QHBoxLayout(tag);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(2);

    QLabel *label = new QLabel(text);
    QPushButton *remove = new QPushButton(deleteIcon, "");
    remove->setFixedSize(16, 16);
    remove->setStyleSheet("QPushButton { border: none; background: transparent; }");

    layout->addWidget(label);
    layout->addWidget(remove);
    tag->setStyleSheet("background: lightgray; border-radius: 6px;");

    tagLayout->addWidget(tag);

    connect(remove, &QPushButton::clicked, this, [=]() {
        removeTag(tag);
        emit valuesChanged();
    });
}

void MultiValueWidget::removeTag(QWidget *tagWidget)
{
    tagLayout->removeWidget(tagWidget);
    tagWidget->deleteLater();
}

QStringList MultiValueWidget::values() const
{
    QStringList list;
    for (int i = 0; i < tagLayout->count(); ++i) {
        QWidget *w = tagLayout->itemAt(i)->widget();
        if (w) {
            list << w->property("tagText").toString();
        }
    }
    return list;
}

void MultiValueWidget::clear()
{
    while (tagLayout->count()) {
        QWidget *w = tagLayout->itemAt(0)->widget();
        removeTag(w);
    }
    emit valuesChanged();
}

void MultiValueWidget::mousePressEvent(QMouseEvent *event)
{
    // Open popup if clicked anywhere in the widget except the popup itself
    if (!popup->isVisible()) {
        showPopup();
    }
    QWidget::mousePressEvent(event);
}
