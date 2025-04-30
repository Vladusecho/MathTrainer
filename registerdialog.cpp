#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "QMessageBox"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStackedLayout>

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
    QString login = ui->le_login->text().trimmed();
    QString password = ui->le_pass->text().trimmed();
    QString repeatPassword = ui->le_repeat_pass->text().trimmed();

    if (login.isEmpty() || password.isEmpty() || repeatPassword.isEmpty()) {
        showWarningMessage("Внимание!", "Пустые поля недопустимы.");
    }
    else if (!checkLogin(login)) {
        showWarningMessage("Ваш логин некорректен!","-Длина от 3 до 20 символов\n-Латинские буквы\n-Цифры\n-Спец. символы (_, -, .))");
    }
    else if (db.isLoginExists(login)) {
        showWarningMessage("Внимание!", "Пользователь с таким логином уже существует!");
    }
    else if (!checkPassword(password)){
        showWarningMessage("Ваш пароль некорректен!","-Длина от 8 до 30 символов\n-Минимум 1 цифра\n-Минимум 1 буква в нижнем регистре\n-Минимум 1 буква в верхнем регистре\n-Минимум 1 спецсимвол");
    }
    else if (password != repeatPassword) {
        showWarningMessage("Внимание!", "Пароли не совпадают.");
    }
    else if (db.registerUser(login, password)){
        showSuccessMessage("Регистрация завершена!");
    }
}

void RegisterDialog::showWarningMessage(QString title, QString text)
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

void RegisterDialog::showSuccessMessage(QString text)
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

bool RegisterDialog::checkPassword(QString password)
{
    static QRegularExpression regex("^(?=.*[0-9])(?=.*[a-z])(?=.*[A-Z])(?=.*[!@#$%^&*()\\[\\]_+\\-=]).{8,30}$");
    if(!regex.match(password).hasMatch()) {
        return false;
    }
    return true;
}

bool RegisterDialog::checkLogin(QString login)
{
    static QRegularExpression regex("^[a-zA-Z][a-zA-Z0-9_.-]{2,19}$");
    if(!regex.match(login).hasMatch()) {
        return false;
    }
    return true;
}

void RegisterDialog::on_btn_back_clicked()
{
    RegisterDialog::close();
}

