#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "databasemanager.h"
#include "digitalbackground.h"
#include "gamelogic.h"
#include "musicplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class User {
private:
    int id;
    QString nickname;
    QString password;
    int lvl;
    int exp;
    int easy;
    int medium;
    int hard;

    const int BASE_EXP_REQUIRED = 30;
    const float EXP_GROWTH_RATE = 1.5f;

public:

    User() {
        this->id = -1;
        this->exp = 0;
        this->lvl = 0;
        this->easy = 0;
        this->medium = 0;
        this->hard = 0;
        this->nickname = "Unknown";
        this->password = "0000";
    }

    QString getNickname() {
        return nickname;
    }

    int getId() {
        return id;
    }

    int getLvl() {
        return lvl;
    }

    int getExp() {
        return exp;
    }

    void setId(int id) {
        this->id = id;
    }

    int getEasy() {
        return this->easy;
    }

    int getMedium() {
        return this->medium;
    }

    int getHard() {
        return this->hard;
    }

    void setMedium(int medium) {
        this->medium = medium;
    }

    void setEasy(int easy) {
        this->easy = easy;
    }

    void setHard(int hard) {
        this->hard = hard;
    }

    void setNickname(QString nickname) {
        this->nickname = nickname;
    }

    void setLvl(int lvl) {
        this->lvl = lvl;
    }

    void addExp(int addedExp) {
        this->exp = this->exp + addedExp;
        int expNeeded = calculateExpForNextLevel(this->lvl);
        if(this->exp >= expNeeded) {
            levelUpUser();
        }
    }

    void levelUpUser() {
        int expNeeded = calculateExpForNextLevel(this->lvl);
        int remainingExp = abs(this->exp - expNeeded);
        this->exp = remainingExp;
        this->lvl++;
    }

    void setAll(int id, QString nick, QString password, int lvl, int exp, int easy, int medium, int hard) {
        this->id = id;
        this->exp = exp;
        this->lvl = lvl;
        this->nickname = nick;
        this->password = password;
        this->easy = easy;
        this->medium = medium;
        this->hard = hard;
    }

    int calculateExpForNextLevel(int currentLevel) {
        return static_cast<int>(BASE_EXP_REQUIRED * pow(EXP_GROWTH_RATE, currentLevel - 1));
    }

    int getRemainingExp() {
        int expNeeded = calculateExpForNextLevel(this->lvl);
        int remainingExp = abs(this->exp - expNeeded);
        return remainingExp;
    }

    User clear() {
        return User();
    }
};

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

    void on_btn_profile_clicked();

    void on_btn_profile_back_clicked();

    void on_btn_sound_clicked();

    void on_btn_sound_2_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_verticalSlider_2_valueChanged(int value);

    void on_btn_back_reg_clicked();

    void on_btn_to_reg_clicked();

    void on_btn_to_start_clicked();

    void on_btn_back_login_clicked();

    void on_btn_back_about_clicked();

    void on_btn_back_lvl_clicked();

    void on_btn_select_easy_clicked();

    void on_btn_select_lvl_clicked();

    void on_btn_back_game_clicked();

    void on_btn_back_finish_clicked();

    void on_btn_back_stat_clicked();

    void on_btn_stats_clicked();

    void on_btn_select_medium_clicked();

    void on_btn_select_hard_clicked();

private:
    Ui::MainWindow *ui;
    User user;
    Database db;
    MusicPlayer musicPlayer;
    QWidget *backgroundContainer;
    DigitalBackground *digitalBg;
    GameLogic *gameLogic;

    template<typename T>
    void createDialog(T dialogType);
    void updateSoundButtonIcon();

    void slideToIndex(QStackedWidget *stack, int newIndex);
    void handleDialogClosed(const QString &username, const QString &password);

    bool updatingSliders = false;

    void showWarningMessage(QString title, QString text);
    void showSuccessMessage(QString text);
    bool checkPassword(QString password);
    bool checkLogin(QString login);
};
#endif // MAINWINDOW_H
