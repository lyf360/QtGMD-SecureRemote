#ifndef SQLITEDATA_H
#define SQLITEDATA_H

#include <QObject> // 必须继承QObject
#include <QList>       // 新增：存储多行数据
#include <QVariantMap> // 新增：存储单行的字段-值映射
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QByteArray>
#include <QDebug> // 必须添加：包含QDebug的完整定义
// 串口参数宏定义（统一设备名）


class sqliteData : public QObject {
    Q_OBJECT // 必须加，支持QObject特性

public:
    QList<QVariantMap> queryAllDeviceStatus();
    // 构造函数（支持父对象，兼容Qt内存管理）
    explicit sqliteData(QObject *parent = nullptr)
        : QObject(parent) {
        // 构造时自动打开数据库（也可单独写openDB函数）
        openDB("device.db"); // 数据库文件名为device.db，可自定义路径
    }

    // 析构函数：关闭数据库
    ~sqliteData() {
        if (m_db.isOpen()) {
            m_db.close();
            qDebug() << "[数据库] 连接已关闭";
        }
    }

    // ----------start---------


    // 1. 新增/更新指纹等级（level参数类型改为QString，接收"S"/"A"/"B"）
    bool updateFingerprintLevel(int fingerId, QString level);
    // 2. 查询指定指纹ID的等级（返回QString："S"/"A"/"B"/""，空串表示未查询到）
    QString queryFingerprintLevel(int fingerId);
    // 3. 查询所有指纹等级数据（level字段改为字符串）
    QList<QVariantMap> queryAllFingerprintLevels();
    // 4. 打印所有指纹数据（显示字符串等级）
    QString printAllFingerprintData();
    // 5 新增：删除所有指纹数据
    bool deleteAllFingerprintData(); // ← 加这行
    // -----------end----------



    // ========== 核心接口：更新设备状态 ==========
    bool updateDeviceStatus(const QString &deviceType, int deviceNum, const QString &status);

    // ========== 核心接口：查询设备状态 ==========
    QString queryDeviceStatus(const QString &deviceType, int deviceNum);

private:
    // 打开数据库（私有：内部初始化用）
    bool openDB(const QString &dbPath) {
        // 初始化SQLite数据库
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(dbPath);

        // 打开数据库
        if (!m_db.open()) {
            qDebug() << "[数据库] 打开失败：" << m_db.lastError().text();
            return false;
        }

        // 初始化表（若不存在则创建）
        initDeviceStatusTable();
        initFingerprintLevelTable(); // 初始化指纹表
        qDebug() << "[数据库] 打开成功（路径：" << dbPath << "）";
        return true;
    }

    // 初始化device_status表（私有：内部用）
    void initDeviceStatusTable() {
        QSqlQuery query(m_db);
        // 创建表：deviceType+deviceNum作为复合主键，避免重复
        QString createSql = R"(
            CREATE TABLE IF NOT EXISTS device_status (
                deviceType TEXT NOT NULL,
                deviceNum INTEGER NOT NULL,
                status TEXT NOT NULL,
                updated_at TEXT NOT NULL,
                PRIMARY KEY (deviceType, deviceNum)
            )
        )";

        if (!query.exec(createSql)) {
            qDebug() << "[数据库] 创建表失败：" << query.lastError().text();
        }
    }


    void initFingerprintLevelTable() {
        QSqlQuery query(m_db);
        QString createSql = R"(
        CREATE TABLE IF NOT EXISTS fingerprint_level (
            id INTEGER NOT NULL, -- 指纹唯一ID（主键）
            level TEXT NOT NULL, -- 等级：S/A/B（字符串类型）
            updated_at TEXT NOT NULL, -- 最后更新时间
            PRIMARY KEY (id)
        )
    )";
        if (!query.exec(createSql)) {
            qDebug() << "[数据库] 创建fingerprint_level表失败：" << query.lastError().text();
        }
    }

private:
    QSqlDatabase m_db; // 数据库连接对象
};

#endif // SQLITEDATA_H
