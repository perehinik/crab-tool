#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");
    app.setWindowIcon(QIcon(":/icon/crab-small.png"));
    QCoreApplication::setApplicationName("CrabTool");

    MainWindow w;
    w.setWindowIcon(QIcon(":/icon/crab-small.png"));
    w.show();

    return app.exec();
}
