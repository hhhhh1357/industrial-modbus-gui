#include "sqlitemanager.h"
#include <QVariant>
#include <QVector>
#include <QSqlRecord>

/**
 * @brief SqliteManager 构造函数
 * @param parent 父对象
 *
 * 使用指定的连接名 "embedded_db" 添加 SQLite 数据库驱动，初始化数据库对象。
 */
SqliteManager::SqliteManager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "embedded_db");
}

/**
 * @brief 析构函数，关闭数据库连接
 */
SqliteManager::~SqliteManager()
{
    closeDatabase();
}

/**
 * @brief 打开数据库文件
 * @param path 数据库文件路径
 * @return 始终返回 true（实际打开结果需通过 m_db.isOpen() 判断）
 *
 * 设置数据库文件名并尝试打开。如果打开失败，可通过 QSqlDatabase 的错误信息获取原因。
 */
bool SqliteManager::openDatabase(const QString &path)
{
    if (m_db.isOpen()) {
            m_db.close();
        }
        m_db.setDatabaseName(path);
        return m_db.open();
}

/**
 * @brief 关闭数据库连接
 *
 * 如果数据库已打开，则关闭它。
 */
void SqliteManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

/**
 * @brief 执行任意 SQL 语句
 * @param sql 要执行的 SQL 语句
 * @return 执行成功返回 true，否则返回 false（此时可通过 m_lastError 获取错误信息）
 *
 * 在数据库已打开的情况下执行 SQL，失败时记录错误信息。
 */
bool SqliteManager::executeQuery(const QString &sql)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

/**
 * @brief 创建"设置"表（表名 [set]）
 * @return 创建成功返回 true，否则 false
 *
 * 表结构：id（自增主键）、str1、str2、time（默认当前时间戳）
 */
bool SqliteManager::createTable_SheZhi()
{
    QString sql = "CREATE TABLE IF NOT EXISTS [set] ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "str1 TEXT, "
                      "str2 TEXT,"
                      "time TIMESTAMP DEFAULT (datetime('now','localtime')))";

    return executeQuery(sql);
}

/**
 * @brief 创建"检测"表（表名 [jianche]）
 * @return 创建成功返回 true，否则 false
 *
 * 表结构：id、v1、v2、value、time
 */
bool SqliteManager::createTable_jianche()
{
    QString sql = "CREATE TABLE IF NOT EXISTS [jianche] ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "v1 TEXT,"
                      "v2 TEXT, "
                      "value1 TEXT,"
                      "sylc TEXT,"
                      "sjlx TEXT,"
                      "time TIMESTAMP DEFAULT (datetime('now','localtime')))";
    return executeQuery(sql);
}

/**
 * @brief 创建"标定"表（表名 [biaoding]）
 * @return 创建成功返回 true，否则 false
 *
 * 表结构：id、bdlx（标定类型）、lc（量程）、nd（浓度）、
 *         bdxgd（标定吸光度）、ecx（二次项）、ycx（一次项）、
 *         csx（常数项）、time（默认当前时间戳）
 */
bool SqliteManager::createTable_BiaoDing()
{
    QString sql = "CREATE TABLE IF NOT EXISTS [biaoding] ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "bdlx TEXT,"          // 标定类型
                      "lc TEXT, "           // 量程
                      "nd TEXT,"            // 浓度
                      "bdxgd TEXT,"         // 标定吸光度
                      "ecx TEXT, "          // 二次项
                      "ycx TEXT,"           // 一次项
                      "csx TEXT,"           // 常数项
                      "time TIMESTAMP DEFAULT (datetime('now','localtime')))";
    return executeQuery(sql);
}

/**
 * @brief 创建"报警"表（表名 [baojing]）
 * @return 创建成功返回 true，否则 false
 *
 * 表结构：id、nr（内容）、time1（触发日期）、time2（恢复日期）
 */
bool SqliteManager::createTable_BaoJing()
{
    QString sql = "CREATE TABLE IF NOT EXISTS [baojing] ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "nr TEXT,"                 // 内容
                      "time1 TIMESTAMP DEFAULT (datetime('now','localtime'))) "; // 触发日期
    return executeQuery(sql);
}

/**
 * @brief 创建"操作"表（表名 [caozuo]）
 * @return 创建成功返回 true，否则 false
 *
 * 表结构：id、czrz（操作日志）、czrq（操作日期）
 */
bool SqliteManager::createTable_CaoZuo()
{
    QString sql = "CREATE TABLE IF NOT EXISTS [caozuo] ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "czrz TEXT,"               // 操作日志
                      "czrq TIMESTAMP DEFAULT (datetime('now','localtime')))";  // 操作日期
    return executeQuery(sql);
}

/**
 * @brief 向指定表中插入一条记录
 * @param tableName 表名
 * @param data      字段名到值的映射（键为字段名，值为字段值）
 * @return 插入成功返回 true，否则 false
 *
 * 根据映射构建 INSERT 语句，使用预编译方式绑定值，防止 SQL 注入。
 */
bool SqliteManager::insertRecord(const QString &tableName, const QMap<QString, QVariant> &data)
{
    if (!m_db.isOpen()) {
        return false;
    }
    if (data.isEmpty()) {
        return false;
    }
    QSqlQuery query(m_db);
    QStringList fields = data.keys();
    QStringList placeholders;

    // 构建占位符列表，如 ":field1, :field2"
    for (const QString &field : fields) {
        placeholders.append(":" + field);
    }

    // 构造 INSERT 语句
    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                  .arg(tableName)
                  .arg(fields.join(", "))
                  .arg(placeholders.join(", "));
    if (!query.prepare(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }
    // 绑定实际值
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

/**
 * @brief 查询所有数据（示例中固定查询 users 表，可能为历史遗留代码）
 * @return 查询结果，每行为 QVector<QVariant>
 *
 * 注意：此函数硬编码查询 "users" 表，可能与实际业务不符，建议根据实际需求修改。
 */
QVector<QVector<QVariant>> SqliteManager::queryAllData()
{
    QVector<QVector<QVariant>> results;
    if (!m_db.isOpen()) return results;
    QSqlQuery query("SELECT * FROM users", m_db);
    while (query.next()) {
        QVector<QVariant> row;
        row << query.value("id") << query.value("name") << query.value("age") << query.value("email");
        results.append(row);
    }
    if (query.lastError().isValid()) {
        m_lastError = query.lastError().text();
    }
    return results;
}

/**
 * @brief 根据条件删除记录
 * @param tableName       表名
 * @param conditionField  条件字段名
 * @param conditionValue  条件字段值
 * @return 删除成功返回 true，否则 false
 *
 * 示例：DELETE FROM tableName WHERE conditionField = conditionValue
 */
bool SqliteManager::deleteRecord(const QString &tableName,
                                 const QString &conditionField,
                                 const QVariant &conditionValue)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    QString sql = QString("DELETE FROM %1 WHERE %2 = :value")
                  .arg(tableName)
                  .arg(conditionField);
    if (!query.prepare(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }
    query.bindValue(":value", conditionValue);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

/**
 * @brief 根据条件查询单条记录的所有字段值
 * @param tableName       表名
 * @param conditionField  条件字段名
 * @param conditionValue  条件字段值
 * @return 记录中各字段值的向量（按 SELECT * 的顺序）
 *
 * 如果存在多条匹配记录，只返回第一条；若无匹配，返回空向量。
 * 错误信息会存储在 m_lastError 中。
 */
QVector<QVariant> SqliteManager::queryRecord(const QString &tableName,
                                             const QString &conditionField,
                                             const QVariant &conditionValue)
{
    QVector<QVariant> result;
    if (!m_db.isOpen()) return result;
    QString sql = QString("SELECT * FROM %1 WHERE %2 = :value")
                     .arg(tableName)
                     .arg(conditionField);
    QSqlQuery query(m_db);
    query.prepare(sql);
    query.bindValue(":value", conditionValue);

    if (query.exec() && query.next()) {
        // 遍历所有字段，将值存入向量
        for (int i = 0; i < query.record().count(); ++i) {
            result.append(query.value(i));
        }
    } else {
        m_lastError = query.lastError().text();
    }
    return result;
}
