#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "databasemanager.h"
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
public:

    User() {
        this->id = -1;
        this->exp = 0;
        this->lvl = 0;
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

    void setNickname(QString nickname) {
        this->nickname = nickname;
    }

    void setLvl(int lvl) {
        this->lvl = lvl;
    }

    void setExp(int exp) {
        this->exp = exp;
    }

    void setAll(int id, QString nick, QString password, int lvl, int exp) {
        this->id = id;
        this->exp = exp;
        this->lvl = lvl;
        this->nickname = nick;
        this->password = password;
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

private:
    Ui::MainWindow *ui;
    User user;
    Database db;
    MusicPlayer musicPlayer;

    template<typename T>
    void createDialog(T dialogType);
    void updateSoundButtonIcon();

    void slideToIndex(QStackedWidget *stack, int newIndex);
    void handleDialogClosed(const QString &username, const QString &password);

    bool updatingSliders = false;
};
#endif // MAINWINDOW_H
