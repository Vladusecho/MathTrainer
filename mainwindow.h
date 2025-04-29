#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_reg_clicked();

    void on_btn_exit_clicked();

    void on_btn_login_clicked();

    void on_btn_about_clicked();

    void on_btn_change_account_clicked();

    void on_btn_exit_game_clicked();

private:
    Ui::MainWindow *ui;

    template<typename T>
    void createDialog(T dialogType);

    void slideToIndex(QStackedWidget *stack, int newIndex);
    void handleDialogClosed();
};
#endif // MAINWINDOW_H
