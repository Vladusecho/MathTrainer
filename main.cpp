#include "mainwindow.h"

#include <QApplication>
#include <QLabel>
#include <QStatusBar>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    QLabel *author = new QLabel("Vladislav Korzun 2025   ", &w);
    w.statusBar()->addPermanentWidget(author);

    w.show();
    return app.exec();
}
