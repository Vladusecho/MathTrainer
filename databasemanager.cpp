#include "databasemanager.h"
#include <cmath>

Database::Database(QObject *parent) : QObject(parent), databaseName("game_database.sqlite")
{
    connectToDatabase();
    createTables();
}

Database::~Database()
{
    if(db.isOpen()) {
        db.close();
    }
}

bool Database::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseName);

    if(!db.open()) {
        qDebug() << "Error: connection with database failed";
        return false;
    } else {
        qDebug() << "Database: connection ok";
        return true;
    }
}

bool Database::createTables()
{
    QSqlQuery query;

    // Users table
    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "login TEXT UNIQUE NOT NULL, "
                              "password TEXT NOT NULL, "
                              "level INTEGER DEFAULT 1, "
                              "exp INTEGER DEFAULT 0)");

    if(!success) {
        qDebug() << "Error creating tables:" << query.lastError();
        return false;
    }

    return true;
}

bool Database::registerUser(const QString &login, const QString &password)
{
    if(login.isEmpty() || password.isEmpty()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (login, password) VALUES (:login, :password)");
    query.bindValue(":login", login);
    query.bindValue(":password", password); // В реальном приложении нужно хэшировать пароль!

    if(!query.exec()) {
        qDebug() << "Register user error:" << query.lastError();
        return false;
    }

    return true;
}

bool Database::authenticateUser(const QString &login, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if(!query.exec()) {
        qDebug() << "Authentication error:" << query.lastError();
        return false;
    }

    if(query.next()) {
        QString storedPassword = query.value(0).toString();
        return (storedPassword == password);
    }

    return false;
}

int Database::getUserLevel(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT level FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if(!query.exec()) {
        qDebug() << "Get user level error:" << query.lastError();
        return -1;
    }

    if(query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}

int Database::getUserExp(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT exp FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if(!query.exec()) {
        qDebug() << "Get user exp error:" << query.lastError();
        return -1;
    }

    if(query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}

int Database::calculateExpForNextLevel(int currentLevel)
{
    // Формула для расчета необходимого опыта для следующего уровня
    return static_cast<int>(BASE_EXP_REQUIRED * pow(EXP_GROWTH_RATE, currentLevel - 1));
}

bool Database::addUserExp(const QString &login, int expToAdd)
{
    if(expToAdd <= 0) return false;

    // Получаем текущие данные пользователя
    int currentExp = getUserExp(login);
    int currentLevel = getUserLevel(login);

    if(currentExp == -1 || currentLevel == -1) return false;

    int newExp = currentExp + expToAdd;
    int expNeeded = calculateExpForNextLevel(currentLevel);

    QSqlQuery query;
    query.prepare("UPDATE users SET exp = :exp WHERE login = :login");
    query.bindValue(":exp", newExp);
    query.bindValue(":login", login);

    if(!query.exec()) {
        qDebug() << "Add exp error:" << query.lastError();
        return false;
    }

    // Проверяем, не заработал ли пользователь достаточно опыта для нового уровня
    if(newExp >= expNeeded) {
        return levelUpUser(login);
    }

    return true;
}

bool Database::levelUpUser(const QString &login)
{
    // Получаем текущий уровень
    int currentLevel = getUserLevel(login);
    if(currentLevel == -1) return false;

    // Получаем текущий опыт
    int currentExp = getUserExp(login);
    if(currentExp == -1) return false;

    // Рассчитываем оставшийся опыт после повышения уровня
    int expNeeded = calculateExpForNextLevel(currentLevel);
    int remainingExp = currentExp - expNeeded;

    QSqlQuery query;
    query.prepare("UPDATE users SET level = level + 1, exp = :exp WHERE login = :login");
    query.bindValue(":exp", remainingExp);
    query.bindValue(":login", login);

    if(!query.exec()) {
        qDebug() << "Level up error:" << query.lastError();
        return false;
    }

    qDebug() << "User" << login << "leveled up to" << (currentLevel + 1);
    return true;
}
