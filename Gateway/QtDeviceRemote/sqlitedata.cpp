#include "sqlitedata.h"

// ========== 实现：更新设备状态 ==========
QList<QVariantMap> sqliteData::queryAllDeviceStatus()
{
    // 定义返回结果列表
    QList<QVariantMap> resultList;

    // 1. 检查数据库是否打开
    if (!m_db.isOpen()) {
        qDebug() << "[查询所有数据] 数据库未打开";
        return resultList; // 返回空列表
    }

    // 2. 准备并执行查询SQL
    QSqlQuery query(m_db);
    QString sql = "SELECT * FROM device_status"; // 查询所有字段
    if (!query.exec(sql)) {
        qDebug() << "[查询所有数据] 执行失败：" << query.lastError().text();
        return resultList; // 返回空列表
    }

    // 3. 遍历结果集，封装每行数据
    while (query.next()) { // 逐行读取
        QVariantMap rowMap; // 存储单行的“字段名-值”
        // 按字段名读取值，封装到map中（字段名和表结构一致）
        rowMap["deviceType"] = query.value("deviceType").toString();
        rowMap["deviceNum"]  = query.value("deviceNum").toInt();
        rowMap["status"]     = query.value("status").toString();
        rowMap["updated_at"] = query.value("updated_at").toString();

        // 将单行数据添加到结果列表
        resultList.append(rowMap);
    }

    // 4. 日志输出查询结果数量
    qDebug() << "[查询所有数据] 成功，共查询到" << resultList.size() << "条数据";
    return resultList;
}

bool sqliteData::updateFingerprintLevel(int fingerId, QString level)
{
    if (!m_db.isOpen()) {
        qDebug() << "[指纹操作] 数据库未打开";
        return false;
    }

    // 校验参数合法性：仅允许"S"/"A"/"B"（忽略大小写，自动转大写）
    QString validLevel = level.trimmed().toUpper();
    if (fingerId < 0 || !(validLevel == "S" || validLevel == "A" || validLevel == "B")) {
        qDebug() << "[指纹操作] 参数无效（ID：" << fingerId << "，等级：" << level << "），仅支持S/A/B";
        return false;
    }

    QSqlQuery query(m_db);
    // INSERT OR REPLACE：存在该指纹ID则更新，不存在则新增（level存字符串）
    query.prepare(R"(
        INSERT OR REPLACE INTO fingerprint_level (id, level, updated_at)
        VALUES (:id, :level, :time)
    )");
    query.bindValue(":id", fingerId);
    query.bindValue(":level", validLevel); // 绑定转换后的合法等级
    query.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    if (!query.exec()) {
        qDebug() << "[指纹操作] 新增/更新失败（ID：" << fingerId << "，等级：" << validLevel << "）：" << query.lastError().text();
        return false;
    }

    QString op = (query.numRowsAffected() == 1) ? "新增" : "更新";
    qDebug() << "[指纹操作]" << op << "成功（ID：" << fingerId << "，等级：" << validLevel << "）";
    return true;
}

QString sqliteData::queryFingerprintLevel(int fingerId)
{
    if (!m_db.isOpen()) {
        qDebug() << "[指纹操作] 数据库未打开";
        return "";
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT level FROM fingerprint_level WHERE id=:id");
    query.bindValue(":id", fingerId);

    if (query.exec() && query.next()) {
        QString level = query.value("level").toString();
        qDebug() << "[指纹操作] 查询成功（ID：" << fingerId << "，等级：" << level << "）";
        return level;
    } else {
        qDebug() << "[指纹操作] 未查询到指纹（ID：" << fingerId << "）：" << query.lastError().text();
        return ""; // 空字符串表示未查询到
    }
}

QList<QVariantMap> sqliteData::queryAllFingerprintLevels()
{
    QList<QVariantMap> resultList;
    if (!m_db.isOpen()) {
        qDebug() << "[查询所有指纹数据] 数据库未打开";
        return resultList;
    }

    QSqlQuery query(m_db);
    QString sql = "SELECT * FROM fingerprint_level ORDER BY id ASC"; // 按ID升序排列
    if (!query.exec(sql)) {
        qDebug() << "[查询所有指纹数据] 执行失败：" << query.lastError().text();
        return resultList;
    }

    // 遍历结果集，封装成“字段-值”映射（level为字符串）
    while (query.next()) {
        QVariantMap rowMap;
        rowMap["id"] = query.value("id").toInt(); // 指纹ID
        rowMap["level"] = query.value("level").toString(); // 等级（S/A/B）
        rowMap["updated_at"] = query.value("updated_at").toString(); // 更新时间
        resultList.append(rowMap);
    }

    qDebug() << "[查询所有指纹数据] 成功，共" << resultList.size() << "条";
    return resultList;
}

QString sqliteData::printAllFingerprintData()
{
    QList<QVariantMap> allFingerprints = queryAllFingerprintLevels();
    if (allFingerprints.isEmpty()) {
        QString emptyMsg = "指纹数据库为空！\n";
        qDebug() << emptyMsg;
        return emptyMsg;
    }

    // 拼接显示字符串（等级显示S/A/B）
    QString displayStr = "===== 指纹数据库内容 =====\n";
    displayStr += QString("共%1条数据\n\n").arg(allFingerprints.size());
    for (const QVariantMap &finger : allFingerprints) {
        displayStr += QString("指纹ID：%1\n").arg(finger["id"].toInt());
        displayStr += QString("等级：%1\n").arg(finger["level"].toString()); // 直接显示字符串等级
        displayStr += QString("最后更新：%1\n").arg(finger["updated_at"].toString());
        displayStr += "------------------------\n";
    }

    // 日志输出 + 返回字符串（用于UI显示）
    qDebug() << displayStr;
    return displayStr;
}

bool sqliteData::deleteAllFingerprintData()
{
    // 1. 检查数据库是否打开
    if (!m_db.isOpen()) {
        qDebug() << "[指纹操作] 删除失败：数据库未打开";
        return false;
    }

    QSqlQuery query(m_db);
    // 2. 仅保留“删除表中所有数据”的核心SQL（保留表结构，删除所有记录）
    QString deleteSql = "DELETE FROM fingerprint_level;";

    // 3. 事务处理（确保删除操作原子执行）
    m_db.transaction();

    // 4. 执行删除SQL（去掉重置自增ID的语句）
    if (!query.exec(deleteSql)) {
        m_db.rollback(); // 执行失败，回滚事务
        qDebug() << "[指纹操作] 删除所有数据失败：" << query.lastError().text();
        return false;
    }

    // 5. 提交事务
    m_db.commit();

    // 6. 日志输出删除结果
    int deletedCount = query.numRowsAffected();
    qDebug() << "[指纹操作] 删除所有指纹数据成功！共删除" << deletedCount << "条记录";
    return true;
}


bool sqliteData::updateDeviceStatus(const QString &deviceType, int deviceNum, const QString &status)
{
    // 检查数据库是否打开
    if (!m_db.isOpen()) {
        qDebug() << "[设备操作] 数据库未打开";
        return false;
    }

    QSqlQuery query(m_db);
    // INSERT OR REPLACE：存在则更新，不存在则新增
    query.prepare(R"(
        INSERT OR REPLACE INTO device_status (deviceType, deviceNum, status, updated_at)
        VALUES (:type, :num, :status, :time)
    )");
    // 绑定参数（无需QSql::Int/QSql::String，Qt自动推导）
    query.bindValue(":type", deviceType);
    query.bindValue(":num", deviceNum);
    query.bindValue(":status", status);
    query.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    // 执行SQL
    if (!query.exec()) {
        qDebug() << "[设备操作] 失败（类型：" << deviceType << "，编号：" << deviceNum << "）：" << query.lastError().text();
        return false;
    }

    // 日志：区分“新增”/“更新”
    QString op = (query.numRowsAffected() == 1) ? "新增" : "更新";
    qDebug() << "[设备操作]" << op << "成功（类型：" << deviceType << "，编号：" << deviceNum << "，状态：" << status << "）";
    return true;
}

// ========== 实现：查询设备状态 ==========
QString sqliteData::queryDeviceStatus(const QString &deviceType, int deviceNum)
{
    // 检查数据库是否打开
    if (!m_db.isOpen()) {
        qDebug() << "[设备操作] 数据库未打开";
        return "";
    }

    QSqlQuery query(m_db);
    // 准备查询SQL
    query.prepare("SELECT status FROM device_status WHERE deviceType=:type AND deviceNum=:num");
    // 绑定参数（修复：删除错误的QSql::String/QSql::Int）
    query.bindValue(":type", deviceType);
    query.bindValue(":num", deviceNum);

    // 执行查询并获取结果
    if (query.exec() && query.next()) {
        QString status = query.value("status").toString();
        qDebug() << "[设备操作] 查询成功（类型：" << deviceType << "，编号：" << deviceNum << "，状态：" << status << "）";
        return status;
    } else {
        qDebug() << "[设备操作] 查询失败（类型：" << deviceType << "，编号：" << deviceNum << "）：" << query.lastError().text();
        return "";
    }
}
