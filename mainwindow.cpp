#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registerdialog.h"
#include "logindialog.h"
#include "aboutdialog.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QMediaPlayer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    musicPlayer.playBackgroundMusic();
    musicPlayer.setVolume(50);

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

    updateSoundButtonIcon();
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
    if (newIndex < 0 || newIndex >= stack->count())
        return;

    QWidget *currentWidget = stack->currentWidget();
    QWidget *nextWidget = stack->widget(newIndex);

    bool slideToRight = (newIndex < stack->currentIndex());

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


void MainWindow::on_btn_sound_clicked()
{
    musicPlayer.toggleMute();
    updateSoundButtonIcon();

    if (musicPlayer.isMuted()) {
        ui->verticalSlider->setValue(0);
        ui->verticalSlider_2->setValue(0);
    } else {
        ui->verticalSlider->setValue(musicPlayer.volume());
        ui->verticalSlider_2->setValue(musicPlayer.volume());
    }
}

void MainWindow::updateSoundButtonIcon() {
    int currentVolume = musicPlayer.volume();
    bool isActuallyMuted = musicPlayer.isMuted() || currentVolume == 0;

    QString iconPath = isActuallyMuted
    ? ":/img/music_off.svg"
    : ":/img/music_on.svg";

    QIcon icon(iconPath);
    ui->btn_sound->setIcon(icon);
    ui->btn_sound->setIconSize(QSize(32, 32));
    ui->btn_sound_2->setIcon(icon);
    ui->btn_sound_2->setIconSize(QSize(32, 32));
}

void MainWindow::on_btn_sound_2_clicked()
{
    MainWindow::on_btn_sound_clicked();
}


void MainWindow::on_verticalSlider_valueChanged(int value)
{
    if (!updatingSliders) {
        updatingSliders = true;
        musicPlayer.setVolume(value);
        ui->verticalSlider_2->setValue(value);  // Синхронизируем второй ползунок
        updatingSliders = false;
    }

    if (value == 0 && !musicPlayer.isMuted()) {
        musicPlayer.toggleMute();
    }

    else if (value > 0 && musicPlayer.isMuted()) {
        musicPlayer.toggleMute();
    }

    updateSoundButtonIcon();

    statusBar()->showMessage(QString("Громкость: %1%").arg(value), 1000);
}


void MainWindow::on_verticalSlider_2_valueChanged(int value)
{
    if (!updatingSliders) {
        updatingSliders = true;
        musicPlayer.setVolume(value);
        ui->verticalSlider->setValue(value);  // Синхронизируем первый ползунок
        updatingSliders = false;
    }

    if (value == 0 && !musicPlayer.isMuted()) {
        musicPlayer.toggleMute();
    }

    else if (value > 0 && musicPlayer.isMuted()) {
        musicPlayer.toggleMute();
    }

    updateSoundButtonIcon();

    statusBar()->showMessage(QString("Громкость: %1%").arg(value), 1000);
}

