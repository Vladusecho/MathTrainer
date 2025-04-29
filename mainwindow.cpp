#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registerdialog.h"
#include "logindialog.h"
#include "aboutdialog.h"
#include <QPropertyAnimation>


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
    LoginDialog newWindow;
    newWindow.setModal(true);
    connect(&newWindow, &LoginDialog::dialogClosed, this, &MainWindow::handleDialogClosed);
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

void MainWindow::handleDialogClosed()
{
    qDebug() << "Диалог закрыт, выполняем метод главного окна";
    this->slideToIndex(ui->mainStack, 1);
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
    this->slideToIndex(ui->mainStack, 0);
}


void MainWindow::on_btn_exit_game_clicked()
{
    MainWindow::on_btn_exit_clicked();
}

