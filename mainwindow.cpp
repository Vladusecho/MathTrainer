#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registerdialog.h"
#include "logindialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_reg_clicked()
{
    RegisterDialog newWindow;
    newWindow.setModal(true);
    newWindow.exec();
}


void MainWindow::on_btn_exit_clicked()
{
    qApp->exit();
}


void MainWindow::on_btn_login_clicked()
{
    LoginDialog newWindow;
    newWindow.setModal(true);
    newWindow.exec();
}


