#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "databasemanager.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_btn_back_clicked();

    void on_btn_to_start_clicked();

private:
    Ui::LoginDialog *ui;
    Database db;

    void showWarningMessage(QString title, QString text);
    void showSuccessMessage(QString text);

signals:
    void dialogClosed(const QString &username, const QString &password);
};

#endif // LOGINDIALOG_H
