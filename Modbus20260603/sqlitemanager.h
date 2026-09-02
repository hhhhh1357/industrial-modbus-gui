#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include "QObject"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "QSqlError"
#include "QString"
#include <QSqlTableModel>

class SqliteManager : public QObject
{
    Q_OBJECT
public:
    explicit SqliteManager(QObject *parent = nullptr);
    ~SqliteManager();

    bool openDatabase(const QString &path);
    void closeDatabase();
    QVector<QVector<QVariant>> queryAllData();
    QSqlDatabase m_db;

    bool executeQuery(const QString &sql);
    bool createTable_SheZhi();
    bool createTable_jianche();
    bool createTable_BiaoDing();
    bool createTable_BaoJing();
    bool createTable_CaoZuo();
    bool insertRecord(const QString &tableName, const QMap<QString, QVariant> &data);
    bool deleteRecord(const QString &tableName, const QString &conditionField,  const QVariant &conditionValue);
    QVector<QVariant> queryRecord(const QString &tableName,const QString &conditionField,const QVariant &conditionValue);

private:
    QString m_lastError;

};

#endif // SQLITEMANAGER_H
