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
    tagLayout->setAlignment(Qt::AlignHCenter | Qt::AlignLeft);

    deleteIcon = QIcon(":/icon/delete.png");

    popup = new QFrame(nullptr, Qt::Popup);
    popup->setFrameShape(QFrame::Box);
    popup->setMaximumHeight(150);
    QVBoxLayout *popupLayout = new QVBoxLayout(popup);
    popupLayout->setContentsMargins(2, 2, 2, 2);

    valueList = new QListWidget(popup);
    valueList->setStyleSheet(R"(
        QListWidget::item {
            color: black;
            background: white;
            padding: 1px;
        }
        QListWidget::item:hover {
            color: black;
            background: lightblue;
        }
    )");
    popupLayout->addWidget(valueList);

    newEntry = new QLineEdit(popup);
    newEntry->setPlaceholderText("Enter new value");
    popupLayout->addWidget(newEntry);

    connect(newEntry, &QLineEdit::returnPressed, this, &MultiValueWidget::handleAddNew);
    connect(valueList, &QListWidget::itemClicked, this, &MultiValueWidget::handleListItemClicked);
}

QSize MultiValueWidget::sizeHint() const {
    QSize size = layout()->sizeHint();
    if (size.height() < MULTI_VALUE_MIN_HEIGHT) {
        size.setHeight(MULTI_VALUE_MIN_HEIGHT);
    }
    return size;
}

void MultiValueWidget::showPopup() {
    QPoint globalBottomLeft = mapToGlobal(QPoint(0, height()));

    QStringList tags = values();
    QList<QPair<QString, int>> items;
    for (auto it = allValues.begin(); it != allValues.end(); ++it) {
        items.append(qMakePair(it.key(), it.value()));
    }

    std::sort(items.begin(), items.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
        return a.second > b.second; // Sort by value descending
    });

    QStringList sortedKeys;
    for (const auto &pair : items) {
        if (!tags.contains(pair.first)) {
            sortedKeys << pair.first;
        }
    }
    valueList->clear();
    if (!sortedKeys.empty()) {
        valueList->addItems(sortedKeys);
    }
    popup->resize(width(), newEntry->height());
    popup->move(globalBottomLeft);
    popup->show();
    newEntry->setFocus();
}

void MultiValueWidget::handleAddNew() {
    QString text = newEntry->text().trimmed();
    if (!text.isEmpty()) {
        addValue(text);
        newEntry->clear();
        popup->hide();
        emit valuesChanged();
    }
}

void MultiValueWidget::handleListItemClicked(QListWidgetItem *item) {
    if (item) {
        addValue(item->text());
        popup->hide();
        emit valuesChanged();
    }
}

void MultiValueWidget::addValue(const QString &text) {
    if (values().contains(text)) {
        return;
    }
    if (!allValues.contains(text)) {
        allValues[text] = 0;
    }
    allValues[text] += 1;

    QWidget *tag = new QWidget(this);
    tag->setProperty("tagText", text);
    tag->setFixedHeight(MULTI_VALUE_TAG_HEIGHT);

    QHBoxLayout *layout = new QHBoxLayout(tag);
    layout->setContentsMargins(6, MULTI_VALUE_HOR_SPACE, 6, MULTI_VALUE_HOR_SPACE);
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

void MultiValueWidget::removeTag(QWidget *tagWidget) {
    QString tagText = tagWidget->property("tagText").toString();
    if (!allValues.contains(tagText)) {
        allValues[tagText] = 0;
    }
    allValues[tagText] -= 1;

    tagLayout->removeWidget(tagWidget);
    tagWidget->deleteLater();
}

QStringList MultiValueWidget::values() const {
    QStringList list;
    for (int i = 0; i < tagLayout->count(); ++i) {
        QWidget *w = tagLayout->itemAt(i)->widget();
        if (w) {
            list << w->property("tagText").toString();
        }
    }
    return list;
}

void MultiValueWidget::setValues(QStringList valList) {
    clear();
    for (int i = 0; i < valList.length(); i++) {
        addValue(valList[i]);
    }
}

void MultiValueWidget::clear() {
    while (tagLayout->count()) {
        QWidget *w = tagLayout->itemAt(0)->widget();
        removeTag(w);
    }
    emit valuesChanged();
}

void MultiValueWidget::mousePressEvent(QMouseEvent *event) {
    // Open popup if clicked anywhere in the widget except the popup itself
    if (!popup->isVisible()) {
        showPopup();
    }
    QWidget::mousePressEvent(event);
}
