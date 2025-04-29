#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "databasemanager.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_btn_to_reg_clicked();
    void on_btn_back_clicked();

private:
    Ui::RegisterDialog *ui;
    DatabaseManager db;
    void showWarningMessage(QString title, QString text);
    void showSuccessMessage(QString text);
    bool checkPassword(QString password);
    bool checkLogin(QString login);
};

#endif // REGISTERDIALOG_H
