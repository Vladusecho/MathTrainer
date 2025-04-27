#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "QMessageBox"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_btn_to_reg_clicked()
{
    QString password = ui->le_pass->text();
    QString repeatPassword = ui->le_repeat_pass->text();

    if (password != repeatPassword) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("Внимание!");
        msg.setInformativeText("Пароли не совпадают.");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setStyleSheet(
            "QMessageBox {background :rgb(170, 255, 255)}"
            "QLabel {color:rgb(158, 79, 255); font-size: 22px}"
            "QPushButton {color:rgb(158, 79, 255); background:rgb(120, 255, 192)}"
        );
        msg.exec();
    }
}

