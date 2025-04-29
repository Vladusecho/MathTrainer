#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registerdialog.h"
#include "logindialog.h"
#include "aboutdialog.h"


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
    createDialog(RegisterDialog());
}


void MainWindow::on_btn_login_clicked()
{
    createDialog(LoginDialog());
}


void MainWindow::on_btn_about_clicked()
{
    createDialog(AboutDialog());
}


template<typename T>
void MainWindow::createDialog(T dialogType)
{
    static_assert(std::is_base_of<QDialog, T>::value,
                  "Template parameter must inherit from QDialog");

    T newWindow;
    newWindow.setModal(true);
    newWindow.exec();
}


void MainWindow::on_btn_exit_clicked()
{
    qApp->exit();
}


