#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.setStyleSheet("background-color:#A9A9A9;");

    w.show();
    return app.exec();
}
