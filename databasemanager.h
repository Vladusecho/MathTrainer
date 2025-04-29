#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

struct UserProfile {
    int level;
    int experience;
};

struct Achievement {
    int id;
    QString name;
    QString description;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager& instance();

    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool registerUser(const QString &username, const QString &password);
    bool loginUser(const QString &login, const QString &password);
    bool addUserExperience(int userId, int exp);
    UserProfile getUserProfile(int userId) const;
    bool updateUserProfile(int userId, const UserProfile &profile);
    bool unlockAchievement(int userId, int achievementId);
    void closeConnection();

    QVector<Achievement> getAchievements() const;
    QVector<Achievement> getUserAchievements(int userId) const;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
private:
    QSqlDatabase m_db;
    mutable QMutex m_mutex;
    QString m_connectionName;  // Храним уникальное имя соединения

    void initDatabase();
    void createTables();
    QSqlDatabase getDatabase() const;
    bool executeQuery(QSqlQuery &query, const QString &queryStr, const QVariantMap &params = {}) const;
    bool checkDatabase() const;
    bool userExists(const QString &username) const;
};

#endif // DATABASE_MANAGER_H
