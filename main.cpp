#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/crab-small.png"));
    QCoreApplication::setApplicationName("CrabTool");
    MainWindow w;
    w.show();
    return app.exec();
}
