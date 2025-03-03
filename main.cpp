#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QWidget *centralWidget = new QWidget(&w);
    w.setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: #2E3440;");
    w.show();
    return a.exec();
}
