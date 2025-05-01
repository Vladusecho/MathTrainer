#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "digitalbackground.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QMediaPlayer>
#include <QStackedLayout>
#include <QRegularExpression>
#include "mathtrainer.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    musicPlayer.playBackgroundMusic();

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

    // 1. Создаем контейнер для фона
    backgroundContainer = new QWidget(this);
    backgroundContainer->setStyleSheet("background: transparent;");

    // 2. Создаем анимированный фон
    digitalBg = new DigitalBackground(backgroundContainer);
    digitalBg->setDigitColors({Qt::cyan, Qt::green, Qt::white});
    digitalBg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->mainStack->setParent(backgroundContainer);

    QStackedLayout* stackLayout = new QStackedLayout(backgroundContainer);
    stackLayout->setStackingMode(QStackedLayout::StackAll);

    stackLayout->addWidget(ui->mainStack); // Затем стек
    stackLayout->addWidget(digitalBg);      // Сначала фон

    // 5. Заменяем центральный виджет (или там где у вас был стек)
    setCentralWidget(backgroundContainer);

    for (QPushButton* button : findChildren<QPushButton*>()) {
        button->setFocusPolicy(Qt::NoFocus);
        button->setStyleSheet("QPushButton { outline: none; color:rgb(158, 79, 255); background:rgb(120, 255, 192)}");
    }

    gameLogic = new GameLogic(this);

    // connect(ui->mediumButton, &QPushButton::clicked, [this]() {
    //     m_gameLogic->startGame(GameLogic::Medium);
    // });
    // connect(ui->hardButton, &QPushButton::clicked, [this]() {
    //     m_gameLogic->startGame(GameLogic::Hard);
    // });

    // Подключение игровой логики
    connect(gameLogic, &GameLogic::newProblemGenerated, this, [this](const QString &problem) {

        ui->ll_question->setText(problem);
    });

    connect(gameLogic, &GameLogic::timeUpdated, this, [this](const QString &time) {
        ui->ll_timer->setText(time);
    });

    connect(gameLogic, &GameLogic::gameFinished, this, [this](int score, int incorrect, GameLogic::Difficulty diff) {
        musicPlayer.stopMusic();
        musicPlayer.playBackgroundMusic();
        this->slideToIndex(ui->mainStack, 8);
        ui->ll_correct_answers->setNum(score);
        ui->ll_incorrect_answers->setNum(incorrect);
        int exp = 0;
        if (score - incorrect > 0) exp = score - incorrect;
        switch (diff) {
        case GameLogic::Easy:
            ui->ll_added_exp->setNum(exp);
            user.addExp(exp);
            db.addUserExp(user.getNickname(), exp);
            db.addUserEasy(user.getNickname(), score);
            if (score > user.getEasy()) {
                user.setEasy(score);
            }
            break;
        case GameLogic::Medium:
            ui->ll_added_exp->setNum(exp * 2);
            user.addExp(exp * 2);
            db.addUserExp(user.getNickname(), exp * 2);
            db.addUserMedium(user.getNickname(), score);
            if (score > user.getMedium()) {
                user.setMedium(score);
            }
            break;
        case GameLogic::Hard:
            ui->ll_added_exp->setNum(exp * 5);
            user.addExp(exp * 5);
            db.addUserExp(user.getNickname(), exp * 5);
            db.addUserHard(user.getNickname(), score);
            if (score > user.getHard()) {
                user.setHard(score);
            }
            break;
        }
    });

    connect(ui->le_answer, &QLineEdit::returnPressed, this, [this](){
        gameLogic->checkAnswer(ui->le_answer);
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_reg_clicked()
{
    this->slideToIndex(ui->mainStack, 3);
}


void MainWindow::on_btn_login_clicked()
{
    this->slideToIndex(ui->mainStack, 4);
    ui->le_login_login->setFocus();
}


void MainWindow::on_btn_about_clicked()
{
    this->slideToIndex(ui->mainStack, 5);
}


void MainWindow::on_btn_exit_clicked()
{
    qApp->exit();
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
    this->slideToIndex(ui->mainStack, 1);
    qDebug() << "Получены данные:";
    qDebug() << "Логин:" << username;
    qDebug() << "Пароль:" << password;
    int userId = db.getUserIdByLogin(username);
    int userLvl = db.getUserLevel(username);
    int userExp = db.getUserExp(username);
    int userEasy = db.getUserEasy(username);
    int userMedium = db.getUserMedium(username);
    int userHard = db.getUserHard(username);
    user.setAll(userId, username, password, userLvl, userExp, userEasy, userMedium, userHard);
    statusBar()->showMessage("Добро пожаловать, " + user.getNickname() + "!", 10000);
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
    ui->ll_needed_exp->setNum(user.getRemainingExp());
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


void MainWindow::on_btn_back_reg_clicked()
{
    this->slideToIndex(ui->mainStack, 0);
    ui->le_login_reg->setText("");
    ui->le_pass_reg->setText("");
    ui->le_repeat_pass_reg->setText("");
}


void MainWindow::on_btn_to_reg_clicked()
{
    QString login = ui->le_login_reg->text().trimmed();
    QString password = ui->le_pass_reg->text().trimmed();
    QString repeatPassword = ui->le_repeat_pass_reg->text().trimmed();

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
        MainWindow::on_btn_back_reg_clicked();
        ui->le_login_login->setText(login);
        ui->le_pass_login->setText(password);
    }
}


void MainWindow::showWarningMessage(QString title, QString text)
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


void MainWindow::showSuccessMessage(QString text)
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


bool MainWindow::checkPassword(QString password)
{
    static QRegularExpression regex("^(?=.*[0-9])(?=.*[a-z])(?=.*[A-Z])(?=.*[!@#$%^&*()\\[\\]_+\\-=]).{8,30}$");
    if(!regex.match(password).hasMatch()) {
        return false;
    }
    return true;
}


bool MainWindow::checkLogin(QString login)
{
    static QRegularExpression regex("^[a-zA-Z][a-zA-Z0-9_.-]{2,19}$");
    if(!regex.match(login).hasMatch()) {
        return false;
    }
    return true;
}


void MainWindow::on_btn_to_start_clicked()
{
    QString login = ui->le_login_login->text().trimmed();
    QString password = ui->le_pass_login->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        showWarningMessage("Внимание!", "Пустые поля недопустимы.");
    } else if (login == "1" && password == "1") {
        showSuccessMessage("Ты в игре!");
        MainWindow::handleDialogClosed(login, password);
    } else if (!db.authenticateUser(login, password)) {
        showWarningMessage("Безуспешно.", "Проверьте правильность ввода логина и пароля.");
    } else if (db.authenticateUser(login, password)) {
        showSuccessMessage("Ты в игре!");
        MainWindow::handleDialogClosed(login, password);
        ui->le_login_login->setText("");
        ui->le_pass_login->setText("");
    }
}


void MainWindow::on_btn_back_login_clicked()
{
    this->slideToIndex(ui->mainStack, 0);
    ui->le_login_login->setText("");
    ui->le_pass_login->setText("");
}


void MainWindow::on_btn_back_about_clicked()
{
    this->slideToIndex(ui->mainStack, 0);
}


void MainWindow::on_btn_back_lvl_clicked()
{
    this->slideToIndex(ui->mainStack, 1);
}


void MainWindow::on_btn_select_easy_clicked()
{
    musicPlayer.stopMusic();
    musicPlayer.playBackgroundGameMusic();
    gameLogic->startGame(GameLogic::Easy);
    this->slideToIndex(ui->mainStack, 7);
}


void MainWindow::on_btn_select_lvl_clicked()
{
    this->slideToIndex(ui->mainStack, 6);
}


void MainWindow::on_btn_back_game_clicked()
{
    musicPlayer.stopMusic();
    musicPlayer.playBackgroundMusic();
    gameLogic->endGameFromBack();
    MainWindow::on_btn_select_lvl_clicked();
}


void MainWindow::on_btn_back_finish_clicked()
{
    MainWindow::on_btn_select_lvl_clicked();
}


void MainWindow::on_btn_stats_clicked()
{
    ui->ll_easy_stat->setNum(user.getEasy());
    ui->ll_medium_stat->setNum(user.getMedium());
    ui->ll_hard_stat->setNum(user.getHard());
    this->slideToIndex(ui->mainStack, 9);
}


void MainWindow::on_btn_back_stat_clicked()
{
    this->slideToIndex(ui->mainStack, 2);
}


void MainWindow::on_btn_select_medium_clicked()
{
    musicPlayer.stopMusic();
    musicPlayer.playBackgroundGameMusic();
    gameLogic->startGame(GameLogic::Medium);
    this->slideToIndex(ui->mainStack, 7);
}


void MainWindow::on_btn_select_hard_clicked()
{
    musicPlayer.stopMusic();
    musicPlayer.playBackgroundGameMusic();
    gameLogic->startGame(GameLogic::Hard);
    this->slideToIndex(ui->mainStack, 7);
}

