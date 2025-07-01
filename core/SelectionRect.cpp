#include "SelectionRect.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPoint>
#include <QtMath>

SelectionRect::SelectionRect(QGraphicsScene *scene, const QRectF rect, qreal scale) {
    this->scale = scale;
    graphicsRect = scene->addRect(rect, linePen);
    buildCorners(scene, rect);
}

SelectionRect::SelectionRect(QGraphicsScene *scene, const QJsonObject &json, qreal scale) {
    this->scale = scale;
    QRectF rect = QRectF(0, 0, 1, 1);
    this->graphicsRect = scene->addRect(rect, linePen);
    buildCorners(scene, rect);
    fromJson(json);
}

void SelectionRect::buildCorners(QGraphicsScene *scene, QRectF rect) {
    corners = {
        rect.topLeft(), rect.topRight(),
        rect.bottomLeft(), rect.bottomRight()
    };
    for (int i = 0; i < corners.size(); i++) {
        QPointF corner = corners[i];
        qreal circleSizeScaled = circleSize * scale;

        QRectF circleRect(
            corner.x() - (circleSizeScaled / 2),
            corner.y() - (circleSizeScaled / 2),
            circleSizeScaled,
            circleSizeScaled
            );

        QGraphicsEllipseItem *ellipse = scene->addEllipse(circleRect, circlePen, circleBrush);
        ellipses.append(ellipse);
    }
}

void SelectionRect::removeFromScene() {
    if (graphicsRect && graphicsRect->scene()) {
        graphicsRect->scene()->removeItem(graphicsRect);
    }
    if (graphicsRect) {
        delete graphicsRect;
        graphicsRect = nullptr;
    }
    for (int i = 0; i < ellipses.size(); i++) {
        if (ellipses[i] && ellipses[i]->scene()) {
            ellipses[i]->scene()->removeItem(ellipses[i]);
        }
        if (ellipses[i]) {
            delete ellipses[i];
            ellipses[i] = nullptr;
        }
    }
}

QJsonObject SelectionRect::toJson() {
    QJsonObject obj;

    obj["type"] = "selection-corners-rect";

    QRectF r = getRect();
    obj["x-top-left"] = qRound(r.topLeft().x());
    obj["y-top-left"] = qRound(r.topLeft().y());
    obj["x-bot-right"] = qRound(r.bottomRight().x());
    obj["y-bot-right"] = qRound(r.bottomRight().y());

    QJsonArray tagArray;
    for (int i = 0; i < tags.length(); i++) {
        tagArray.append(tags[i]);
    }
    obj["tags"] = tagArray;

    return obj;
}

void SelectionRect::fromJson(const QJsonObject &json) {
    if (json["type"].toString() != "selection-corners-rect") {
        return;
    }
    QRectF rect(
        QPointF(json["x-top-left"].toDouble(), json["y-top-left"].toDouble()),
        QPointF(json["x-bot-right"].toDouble(), json["y-bot-right"].toDouble())
        );

    QJsonArray tagArray = json["tags"].toArray();
    for (int i = 0; i < tagArray.size(); ++i) {
        const QJsonValue &value = tagArray.at(i);
        if (value.isString()) {
            tags.append(value.toString());
        }
    }

    setRect(rect);
}

void SelectionRect::setRect(QRectF rect) {
    // Update rectangle
    graphicsRect->setRect(rect);
    // Update circles
    corners = {
        rect.topLeft(), rect.topRight(),
        rect.bottomLeft(), rect.bottomRight()
    };

    for (int i = 0; i < ellipses.size(); i++) {
        QPointF corner = corners[i];
        qreal circleSizeScaled = circleSize * scale;
        qreal circleStartX = corner.x() - (circleSizeScaled / 2);
        qreal circleStartY = corner.y() - (circleSizeScaled / 2);
        QRectF circleRect(circleStartX, circleStartY, circleSizeScaled, circleSizeScaled);
        ellipses[i]->setRect(circleRect);
    }
}

void SelectionRect::activate() {
    for (int i = 0; i < ellipses.size(); i++) {
        if (ellipses[i] && ellipses[i]->scene()) {
            ellipses[i]->setPen(circlePenActive);
            ellipses[i]->setBrush(circleBrushActive);
        }
    }
}

void SelectionRect::deactivate() {
    for (int i = 0; i < ellipses.size(); i++) {
        if (ellipses[i] && ellipses[i]->scene()) {
            ellipses[i]->setPen(circlePen);
            ellipses[i]->setBrush(circleBrush);
        }
    }
}

QRectF SelectionRect::getRect() {
    return graphicsRect->rect();
}

void SelectionRect::setScale(qreal scale) {
    this->scale = scale;
    setRect(graphicsRect->rect());
    linePen.setWidthF(lineWidth * scale);
    graphicsRect->setPen(linePen);
}

QPointF *SelectionRect::getCornerPoint(QPointF point) {
    for (int i = 0; i < corners.size(); i++) {
        if (abs(QLineF(corners[i], point).length()) < circleSize / 2 * scale) {
            return &corners[i];
        }
    }
    return nullptr;
}

QPointF *SelectionRect::getOppositePoint(QPointF point) {
    for (int i = 0; i < corners.size(); i++) {
        if (std::round(corners[i].x() * 10000) != std::round(point.x() * 10000) &&
            std::round(corners[i].y() * 10000) != std::round(point.y() * 10000)) {
            return &corners[i];
        }
    }
    // In case rectangle forms a line
    for (int i = 0; i < corners.size(); i++) {
        if (std::round(corners[i].x() * 10000) != std::round(point.x() * 10000) ||
            std::round(corners[i].y() * 10000) != std::round(point.y() * 10000)) {
            return &corners[i];
        }
    }
    // In case all points in rectangle are same
    return &corners[0];
}

qreal SelectionRect::getVisibleArea() {
    QRectF rect = getRect();
    qreal vertical = QLineF(rect.topLeft(), rect.topRight()).length() / scale;
    qreal horisontal = QLineF(rect.topLeft(), rect.bottomLeft()).length() / scale;
    return vertical * horisontal;
}

SelectionRect::~SelectionRect(){
    removeFromScene();
}
