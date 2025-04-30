#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registerdialog.h"
#include "logindialog.h"
#include "aboutdialog.h"
#include <QMessageBox>
#include <QPropertyAnimation>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLabel *author = new QLabel("Vladislav Korzun 2025");
    author->setStyleSheet(
        "color: rgb(158, 79, 255);"
        "font-size: 16px;"
        "font-family: Segoe UI;"
        "font-weight: bold;"
        "margin-right: 1px;"
        );
    statusBar()->addPermanentWidget(author);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->setStyleSheet(
        "QStatusBar {"
        "color: rgb(158, 79, 255);"
        "font-size: 16px;"
        "font-family: Segoe UI;"
        "font-weight: bold;"
        "}"
        );
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
    LoginDialog newWindow;
    connect(&newWindow, &LoginDialog::dialogClosed,
            this, &MainWindow::handleDialogClosed);
    newWindow.setModal(true);
    newWindow.exec();
}


void MainWindow::on_btn_about_clicked()
{
    createDialog(AboutDialog());
}

void MainWindow::slideToIndex(QStackedWidget *stack, int newIndex)
{
    // Проверка на валидность индекса
    if (newIndex < 0 || newIndex >= stack->count())
        return;

    // Текущий и следующий виджеты
    QWidget *currentWidget = stack->currentWidget();
    QWidget *nextWidget = stack->widget(newIndex);

    // Определяем направление анимации
    bool slideToRight = (newIndex < stack->currentIndex()); // назад

    // Позиционируем следующий виджет
    int offsetX = currentWidget->width();
    nextWidget->setGeometry(
        slideToRight ? -offsetX : offsetX,
        0,
        currentWidget->width(),
        currentWidget->height()
        );
    nextWidget->show();
    nextWidget->raise();

    // Анимация для текущего виджета (уход)
    QPropertyAnimation *animCurrent = new QPropertyAnimation(currentWidget, "geometry");
    animCurrent->setDuration(300);
    animCurrent->setEasingCurve(QEasingCurve::OutQuad);
    animCurrent->setEndValue(
        QRect(
            slideToRight ? offsetX : -offsetX,
            0,
            currentWidget->width(),
            currentWidget->height()
            )
        );

    // Анимация для следующего виджета (вход)
    QPropertyAnimation *animNext = new QPropertyAnimation(nextWidget, "geometry");
    animNext->setDuration(300);
    animNext->setEasingCurve(QEasingCurve::OutQuad);
    animNext->setEndValue(currentWidget->geometry());

    // Устанавливаем новый индекс после завершения анимации
    connect(animNext, &QPropertyAnimation::finished, this, [=]() {
        stack->setCurrentIndex(newIndex);
        currentWidget->setGeometry(animNext->endValue().toRect());
    });

    // Запускаем анимации
    animCurrent->start();
    animNext->start();
}

void MainWindow::handleDialogClosed(const QString &username, const QString &password)
{
    qDebug() << "Диалог закрыт, выполняем метод главного окна";
    this->slideToIndex(ui->mainStack, 1);
    qDebug() << "Получены данные:";
    qDebug() << "Логин:" << username;
    qDebug() << "Пароль:" << password;
    int userId = db.getUserIdByLogin(username);
    int userLvl = db.getUserLevel(username);
    int userExp = db.getUserExp(username);
    user.setAll(userId, username, password, userLvl, userExp);
    statusBar()->showMessage("Добро пожаловать, " + user.getNickname() + "!", 10000);
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


void MainWindow::on_btn_change_account_clicked()
{
    statusBar()->clearMessage();
    this->slideToIndex(ui->mainStack, 0);
}


void MainWindow::on_btn_exit_game_clicked()
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText("Выход");
    msg.setInformativeText("Вы действительно хотите выйти?");
    QPushButton *no = msg.addButton("Нет!", QMessageBox::DestructiveRole);
    QPushButton *yes = msg.addButton("Да:(", QMessageBox::AcceptRole);
    msg.setStyleSheet(
        "QMessageBox {background :rgb(170, 255, 255)}"
        "QLabel {color:rgb(158, 79, 255); font-size: 22px; font-family: Segoe UI; font-weight: bold;}"
        "QPushButton {color:rgb(158, 79, 255); background:rgb(120, 255, 192); font-family: Segoe UI; font-weight: bold; font-size: 22px;}"
        );
    msg.exec();
    if (msg.clickedButton() == yes) {
        MainWindow::on_btn_exit_clicked();
    } else if (msg.clickedButton() == no) {
        msg.close();
    }
}


void MainWindow::on_btn_profile_clicked()
{
    ui->ll_nickname->setText(user.getNickname());
    ui->ll_lvl->setNum(user.getLvl());
    ui->ll_exp->setNum(user.getExp());
    ui->ll_needed_exp->setNum(db.calculateExpForNextLevel(user.getLvl()));
    this->slideToIndex(ui->mainStack, 2);
}


void MainWindow::on_btn_profile_back_clicked()
{
    this->slideToIndex(ui->mainStack, 1);
}

