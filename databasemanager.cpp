#include "databasemanager.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/app_database.sqlite";

    qDebug() << "Database path:" << dbPath;

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "Cannot open database";
        return;
    }
}

DatabaseManager::~DatabaseManager()
{
    QMutexLocker locker(&m_mutex);
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QSqlDatabase DatabaseManager::getDatabase() const
{
    QMutexLocker locker(&m_mutex);
    return m_db;
}

void DatabaseManager::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("game_database.sqlite");

    if (!m_db.open()) {
        qCritical() << "Cannot open database";
    }
}

void DatabaseManager::createTables()
{
    QFile schemaFile(":/db/database_schema.sql");
    if (!schemaFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open schema file";
        return;
    }

    QStringList schemaQueries = QString(schemaFile.readAll()).split(';');

    QSqlQuery query;
    for (const QString &q : schemaQueries) {
        if (q.trimmed().isEmpty()) continue;
        if (!query.exec(q)) {
            qCritical() << "Failed to execute query";
        }
    }
}

bool DatabaseManager::executeQuery(QSqlQuery &query, const QString &queryStr, const QVariantMap &params) const
{
    QSqlDatabase db = getDatabase();
    query = QSqlQuery(db);

    if (!query.prepare(queryStr)) {
        qCritical() << "Prepare failed";
        return false;
    }

    // Привязываем параметры
    for (auto it = params.begin(); it != params.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qCritical() << "Query failed";
        qCritical() << "Executed query:" << query.lastQuery();
        qCritical() << "Bound values:" << query.boundValues();
        return false;
    }

    return true;
}

bool DatabaseManager::userExists(const QString &username) const
{
    QSqlQuery query;
    QVariantMap params;
    params[":username"] = username;

    if (!executeQuery(query, "SELECT COUNT(*) FROM users WHERE username = :username", params)) {
        return false;
    }

    return query.next() && query.value(0).toInt() > 0;
}

bool DatabaseManager::registerUser(const QString &username, const QString &password)
{
    if (userExists(username)) {
        qWarning() << "User already exists:" << username;
        return false;
    }

    QSqlQuery query;
    QVariantMap params;
    params[":username"] = username;
    params[":password"] = password;

    return executeQuery(query,
                        "INSERT INTO users (username, password) VALUES (:username, :password)",
                        params);
}

// bool DatabaseManager::loginUser(const QString &username, const QString &password)
// {
//     QSqlQuery query;
//     query.prepare(
//         "SELECT id, password FROM users "
//         "WHERE username = :username"
//         );
//     query.bindValue(":username", username);

//     if (!executeQuery(query) || !query.next()) {
//         return false;
//     }

//     QString existPassword = query.value("password").toString();

//     return existPassword == password;
// }

// UserProfile DatabaseManager::getUserProfile(int userId) const
// {
//     UserProfile profile;
//     QSqlQuery query;
//     query.prepare(
//         "SELECT level, experience"
//         "FROM user_profiles WHERE user_id = :user_id"
//         );
//     query.bindValue(":user_id", userId);

//     if (executeQuery(query) && query.next()) {
//         profile.level = query.value("level").toInt();
//         profile.experience = query.value("experience").toInt();
//     }

//     return profile;
// }

// bool DatabaseManager::addUserExperience(int userId, int exp)
// {
//     UserProfile profile = getUserProfile(userId);
//     profile.experience += exp;

//     int expNeeded = profile.level * 100;
//     if (profile.experience >= expNeeded) {
//         profile.level++;
//         profile.experience -= expNeeded;
//     }

//     return updateUserProfile(userId, profile);
// }

// bool DatabaseManager::updateUserProfile(int userId, const UserProfile &profile)
// {
//     QSqlQuery query;
//     query.prepare(
//         "UPDATE user_profiles SET "
//         "level = :level, "
//         "experience = :exp, "
//         "WHERE user_id = :user_id"
//         );
//     query.bindValue(":level", profile.level);
//     query.bindValue(":exp", profile.experience);

//     query.bindValue(":user_id", userId);

//     return executeQuery(query);
// }

// QVector<Achievement> DatabaseManager::getAchievements() const
// {
//     QVector<Achievement> achievements;
//     QSqlQuery query("SELECT id, name, description FROM achievements");

//     while (query.next()) {
//         Achievement a;
//         a.id = query.value("id").toInt();
//         a.name = query.value("name").toString();
//         a.description = query.value("description").toString();
//         achievements.append(a);
//     }

//     return achievements;
// }

// QVector<Achievement> DatabaseManager::getUserAchievements(int userId) const
// {
//     QVector<Achievement> achievements;
//     QSqlQuery query;
//     query.prepare(
//         "SELECT a.id, a.name, a.description "
//         "FROM achievements a "
//         "JOIN user_achievements ua ON a.id = ua.achievement_id "
//         "WHERE ua.user_id = :user_id"
//         );
//     query.bindValue(":user_id", userId);

//     if (executeQuery(query)) {
//         while (query.next()) {
//             Achievement a;
//             a.id = query.value("id").toInt();
//             a.name = query.value("name").toString();
//             a.description = query.value("description").toString();
//             achievements.append(a);
//         }
//     }

//     return achievements;
// }

// bool DatabaseManager::unlockAchievement(int userId, int achievementId)
// {
//     QSqlQuery query;
//     query.prepare(
//         "INSERT OR IGNORE INTO user_achievements (user_id, achievement_id) "
//         "VALUES (:user_id, :achievement_id)"
//         );
//     query.bindValue(":user_id", userId);
//     query.bindValue(":achievement_id", achievementId);

//     return executeQuery(query);
// }

