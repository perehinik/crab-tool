#include <QPalette>
#include <QGridLayout>
#include <QToolButton>
#include <QPushButton>
#include <QCursor>
#include <QMenuBar>
#include <QToolBar>

#include "ToolboxWidget.h"
#include "ToolboxButtonWidget.h"
#include "InfoPopup.h"

ToolboxWidget::ToolboxWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    layout->setSpacing(0);

    createProjectButton = new ToolboxButton(QIcon(":/icon/project-file.png"), "New Project", 32, 20, this);
    openProjectButton = new ToolboxButton(QIcon(":/icon/project-file-open.png"), "Open Project", 32, 20, this);
    saveButton = new ToolboxButton(QIcon(":/icon/save.png"), "Save Project", 32, 24, this);
    openImagesButton = new ToolboxButton(QIcon(":/icon/images.png"), "Open Images", 32, 24, this);
    openDirButton = new ToolboxButton(QIcon(":/icon/folder-fill.png"), "Open Directory", 32, 24, this);

    handToolButton = new ToolboxButton(QIcon(":/icon/drag.png"), "Move", 32, 24, this);
    handToolButton->action->setCheckable(true);

    zoomInToolButton = new ToolboxButton(QIcon(":/icon/zoom-in.png"), "Zoom In", 32, 24, this);
    zoomOutToolButton = new ToolboxButton(QIcon(":/icon/zoom-out.png"), "Zoom Out", 32, 24, this);
    zoomToExtentsToolButton = new ToolboxButton(QIcon(":/icon/zoom-to-extents.png"), "Zoom To Extents", 32, 24, this);

    ToolboxButton *infoButton = new ToolboxButton(QIcon(":/icon/info.png"), "Info", 32, 24, this);
    connect(infoButton->action, &QAction::triggered, this, &ToolboxWidget::showInfoPopup);

    menuBar = new QMenuBar(this);
    QMenu *fileMenu = menuBar->addMenu("File");
    fileMenu->addAction(createProjectButton->action);
    fileMenu->addAction(openProjectButton->action);
    fileMenu->addAction(saveButton->action);
    fileMenu->addAction(openImagesButton->action);
    fileMenu->addAction(openDirButton->action);

    QMenu *toolsMenu = menuBar->addMenu("Tools");
    toolsMenu->addAction(handToolButton->action);
    toolsMenu->addAction(zoomInToolButton->action);
    toolsMenu->addAction(zoomOutToolButton->action);
    toolsMenu->addAction(zoomToExtentsToolButton->action);

    QMenu *helpMenu = menuBar->addMenu("Help");
    helpMenu->addAction(infoButton->action);

    setStyleSheet(R"(
        ToolboxWidget {
            border: 1px solid lightgrey
        }

        QMenu::item:checked {
            background-color: #a0d0ff;
        }
    )");

    QToolBar *toolbar = new QToolBar(this);
    toolbar->addWidget(createProjectButton);
    toolbar->addWidget(openProjectButton);
    toolbar->addWidget(openImagesButton);
    toolbar->addWidget(openDirButton);
    toolbar->addWidget(saveButton);
    toolbar->addSeparator();
    toolbar->addWidget(handToolButton);
    toolbar->addWidget(zoomInToolButton);
    toolbar->addWidget(zoomOutToolButton);
    toolbar->addWidget(zoomToExtentsToolButton);
    toolbar->addSeparator();
    toolbar->addWidget(infoButton);

    layout->setMenuBar(menuBar);
    layout->addWidget(toolbar, 0, 0);
}

void ToolboxWidget::showInfoPopup() {
    InfoPopup* popup = new InfoPopup(this);
    popup->exec();
    delete popup;
}
