#include "logindialog.h"
#include "ui_logindialog.h"
#include "QMessageBox"


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_btn_back_clicked()
{
    LoginDialog::close();
}


void LoginDialog::on_btn_to_start_clicked()
{
    QString login = ui->le_login->text().trimmed();
    QString password = ui->le_pass->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        showWarningMessage("Внимание!", "Пустые поля недопустимы.");
    } else if (login == "1" && password == "1") {
        showSuccessMessage("Ты в игре!");
        emit dialogClosed(ui->le_login->text(), ui->le_pass->text());
        LoginDialog::close();
    } else if (!db.authenticateUser(login, password)) {
        showWarningMessage("Безуспешно.", "Проверьте правильность ввода логина и пароля.");
    } else if (db.authenticateUser(login, password)) {
        showSuccessMessage("Ты в игре!");
        emit dialogClosed(ui->le_login->text(), ui->le_pass->text());
        LoginDialog::close();
    }
}

void LoginDialog::showWarningMessage(QString title, QString text)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(title);
    msg.setInformativeText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setStyleSheet(
        "QMessageBox {background :rgb(170, 255, 255)}"
        "QLabel {color:rgb(158, 79, 255); font-size: 22px; font-family: Segoe UI; font-weight: bold;}"
        "QPushButton {color:rgb(158, 79, 255); background:rgb(120, 255, 192); font-family: Segoe UI; font-weight: bold; font-size: 22px;}"
        );
    msg.exec();
}

void LoginDialog::showSuccessMessage(QString text)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setText("Успешно!");
    msg.setInformativeText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setStyleSheet(
        "QMessageBox {background :rgb(170, 255, 255)}"
        "QLabel {color:rgb(158, 79, 255); font-size: 22px; font-family: Segoe UI; font-weight: bold;}"
        "QPushButton {color:rgb(158, 79, 255); background:rgb(120, 255, 192); font-family: Segoe UI; font-weight: bold; font-size: 22px;}"
        );
    msg.exec();
}

