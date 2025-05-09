#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool connectToDatabase();
    bool createTables();

    // User management
    bool isLoginExists(const QString &login);
    int getUserIdByLogin(const QString &login);
    bool registerUser(const QString &login, const QString &password);
    bool authenticateUser(const QString &login, const QString &password);
    bool changeNick(int id, const QString &newNick);
    QString getUserNick(int id);

    int getUserLevel(const QString &login);
    int getUserExp(const QString &login);
    int getUserHard(const QString &login);
    int getUserMedium(const QString &login);
    int getUserEasy(const QString &login);
    bool addUserExp(const QString &login, int expToAdd);
    bool addUserEasy(const QString &login, int easy);
    bool addUserMedium(const QString &login, int medium);
    bool addUserHard(const QString &login, int hard);
    bool levelUpUser(const QString &login);

    int calculateExpForNextLevel(int currentLevel);


private:
    QSqlDatabase db;
    QString databaseName;

    const int BASE_EXP_REQUIRED = 30;
    const float EXP_GROWTH_RATE = 1.5f;

};

#endif // DATABASE_MANAGER_H
