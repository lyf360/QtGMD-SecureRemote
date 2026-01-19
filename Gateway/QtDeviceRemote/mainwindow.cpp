#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QVBoxLayout>

#define PASSWD_4BIT "4321"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , loginTimer(nullptr)  // 初始化指针
    , remainingTime(0)     // 初始剩余时间0
{
    ui->setupUi(this);



    ui->plainTextEdit_4->setStyleSheet(R"(
    #plainTextEdit_5 QScrollBar:vertical {
        width: 70px;
        background: #f5f5f5;
        border-radius: 20px;
    }
    #plainTextEdit_5 QScrollBar::handle:vertical {
        background: #666;
        border-radius: 20px;
        min-height: 60px;
        margin: 5px 0;
    }
    #plainTextEdit_5 QScrollBar::add-line:vertical,
    #plainTextEdit_5 QScrollBar::sub-line:vertical {
        height: 0;
    }
)");

    fingerPrint = new FingerPrintDevice("/dev/ttymxc1",57600);
    sqlitedata = new sqliteData(this); // 父对象设为MainWindow


    mqttdevice =new SmartDeviceMqtt("/iot/smartdevice/doorlock","/iot/smartdevice/doorlock/ack");   //

    // 2. 创建子线程和工作对象
    m_mqttThread = new QThread(this);
    m_mqttWorker = new MqttWorker(mqttdevice);  // 传入MQTT对象（不转移所有权）


    // 3. 将工作对象移动到子线程
    m_mqttWorker->moveToThread(m_mqttThread);

    // 4. 绑定信号槽（跨线程安全传递消息）
    // 子线程接收消息后 → 主线程处理并更新UI
    connect(m_mqttWorker, &MqttWorker::messageReceived,
            this, &MainWindow::onMessageReceived);
    // 子线程发生错误 → 主线程显示错误
    connect(m_mqttWorker, &MqttWorker::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    // 线程启动后 → 开始接收消息
    connect(m_mqttThread, &QThread::started,
            m_mqttWorker, &MqttWorker::startReceiving);
    // 线程结束后 → 自动销毁工作对象（避免内存泄漏）
    connect(m_mqttThread, &QThread::finished,
            m_mqttWorker, &MqttWorker::deleteLater);

    // 5. 启动子线程（开始接收消息）
    m_mqttThread->start();



    lasttext = ui->plainTextEdit;
    usertext = ui->plainTextEdit_2;
    page1test = ui->plainTextEdit_3;
    deviceAckShow = ui->plainTextEdit_4;


    // 1. 必须固定窗口大小（嵌入式屏幕常用800x480，根据你的屏幕修改）
    this->setFixedSize(800, 480);

    // 2. 让stackedWidget铺满窗口（避免布局混乱导致按钮不可见）
    this->setCentralWidget(ui->stackedWidget);
    ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 3. 初始化定时器（确保初始是停止状态）
    loginTimer = new QTimer(this);
    loginTimer->setInterval(1000);
    loginTimer->setSingleShot(false);
    connect(loginTimer, &QTimer::timeout, this, &MainWindow::updateLoginTimer);
    loginTimer->stop();  // 显式停止，避免意外激活

    // // 4. 加载背景图（保持原有逻辑）
    // QPixmap bgPixmap(":/backgroundasd.png");
    // bgPixmap = bgPixmap.scaled(
    //     this->width(), this->height(),
    //     Qt::IgnoreAspectRatio,
    //     Qt::FastTransformation
    //     );
    // QPalette palette = ui->stackedWidget->palette();
    // palette.setBrush(QPalette::Background, bgPixmap);
    // ui->stackedWidget->setPalette(palette);
    // ui->stackedWidget->setAutoFillBackground(true);

    // 5. 子页面透明+无边距布局（避免遮挡按钮）
    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        QWidget *page = ui->stackedWidget->widget(i);
        page->setStyleSheet("background-color: transparent;");
        // 给子页面加无边距布局，确保按钮铺满页面
        QVBoxLayout *pageLayout = new QVBoxLayout(page);
        pageLayout->setMargin(0);
        pageLayout->setSpacing(0);
        page->setLayout(pageLayout);
    }

    // 6. 样式表（添加按钮焦点样式，确保点击有效）
    this->setStyleSheet(R"(
    /* 按钮样式：浅灰色背景+黑色文字，适配白色背景 */
    QPushButton {
        background-color: #E5E5E5;  /* 主浅灰色背景 */
        color: #000000;  /* 纯黑色文字 */
        margin: 0px;
        padding: 0px;
        border: none;
        border-radius: 6px;  /* 圆角区分边界 */
        font-size: 16px;
        font-weight: bold;  /* 加粗突出按钮文字 */
    }
    /* 按钮按下状态：加深浅灰色，用颜色深浅替代box-shadow反馈 */
    QPushButton:pressed {
        background-color: #C5C5C5;  /* 比之前更深的浅灰色，反馈更明显 */
        color: #000000;  /* 保持黑色文字 */
    }
    /* 按钮禁用状态：浅灰白色+深灰色文字，明确不可用 */
    QPushButton:disabled {
        background-color: #F0F0F0;  /* 接近白色的浅灰 */
        color: #888888;  /* 深灰色文字 */
        font-weight: normal;
    }
    /* 去掉焦点边框，避免影响外观 */
    QPushButton:focus {
        outline: none;
    }

    /* 标签样式：纯黑色文字，与白色背景强区分 */
    QLabel {
        background-color: transparent;
        color: #000000;  /* 纯黑色文字 */
        font-size: 14px;
        padding: 4px 0;  /* 上下内边距，避免文字贴边 */
    }

    /* 文本框样式：浅灰色背景+黑色文字，与白色背景区分 */
    QPlainTextEdit {
        background-color: #F0F0F0;  /* 浅灰色背景 */
        color: #000000;  /* 纯黑色文字 */
        margin: 0px;
        padding: 10px;
        border: 1px solid #DDDDDD;  /* 浅灰色边框，明确范围 */
        border-radius: 4px;
        font-size: 14px;
    }
    /* 文本框聚焦状态：加深边框+微浅背景，提示聚焦 */
    QPlainTextEdit:focus {
        background-color: #F5F5F5;  /* 略深一点的浅灰色 */
        color: #000000;  /* 保持黑色文字 */
        border-color: #CCCCCC;  /* 加深边框颜色，明确聚焦 */
        outline: none;
    }
    /* 文本框禁用状态：更浅的灰色+深灰文字 */
    QPlainTextEdit:disabled {
        background-color: #F8F8F8;  /* 接近白色的浅灰 */
        color: #888888;  /* 深灰色文字 */
        border-color: #EEEEEE;
    }
)");
    // 7. 初始启用所有按键（关键！确保启动时按钮可用）
    setLoginButtonsEnabled(true);

    bool ok = fingerPrint->sendHandshackProtocol();
    if(!ok){
        cout << "握手失败"<< endl;

    }
}

MainWindow::~MainWindow()
{

    m_mqttThread->quit();
    m_mqttThread->wait();
    if(mqttdevice)
    {
        delete mqttdevice;
    }

    if(sqlitedata)
    {
        delete sqlitedata;
    }


    delete ui;
}

// 完善：包含所有交互按钮的启用/禁用（之前遗漏了返回、登录选择按钮）
void MainWindow::setLoginButtonsEnabled(bool enabled)
{
    // 数字按键
    ui->pushButton_4->setEnabled(enabled);
    ui->pushButton_5->setEnabled(enabled);
    ui->pushButton_6->setEnabled(enabled);
    ui->pushButton_7->setEnabled(enabled);
    // 删除键
    ui->pushButton_3->setEnabled(enabled);
    // 登录确认按钮
    ui->pushButton->setEnabled(enabled);
    // 密码输入框
    lasttext->setEnabled(enabled);
    // 补充：返回按钮（必须启用，否则无法返回）
    ui->pushButton_2->setEnabled(true);  // 返回按钮始终可用
    // 补充：登录选择按钮（密码/指纹登录，始终可用）
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
}



void MainWindow::on_pushButton_15_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



// 定时器更新函数（保持原有逻辑）
void MainWindow::updateLoginTimer()
{
    remainingTime--;
    if (remainingTime > 0) {
        usertext->setPlainText(QString("密码错误！还剩 %1 秒后可重新输入").arg(remainingTime));
    } else {
        loginTimer->stop();
        usertext->setPlainText("请输入4位密码");
        setLoginButtonsEnabled(true);
        lasttext->clear();
    }
}

// 登录按钮点击（保持原有逻辑）
void MainWindow::on_pushButton_clicked()
{
    QString inputPwd = lasttext->toPlainText().trimmed();
    usertext->clear();
    if (inputPwd.isEmpty()) {
        usertext->setPlainText("请输入4位密码");
        return;
    }
    if (inputPwd.length() != 4) {
        usertext->setPlainText("密码必须是4位数字！");
        return;
    }
    if (inputPwd == PASSWD_4BIT) {
        usertext->setPlainText("密码正确，进入下一页！");
        ui->stackedWidget->setCurrentIndex(2);
        initPlainTextEdit();
        lasttext->clear();
    } else {
        remainingTime = 30;
        usertext->setPlainText(QString("密码错误！还剩 %1 秒后可重新输入").arg(remainingTime));
        setLoginButtonsEnabled(false);  // 错误时禁用输入相关按钮
        loginTimer->start();
        lasttext->clear();
    }
}

// 返回按钮点击（补充：停止定时器，恢复输入功能）
void MainWindow::on_pushButton_2_clicked()
{
    if (loginTimer->isActive()) {
        loginTimer->stop();  // 停止倒计时
        setLoginButtonsEnabled(true);  // 恢复按钮可用
    }
    ui->stackedWidget->setCurrentIndex(0);
    page1test->clear();
    usertext->clear();
    lasttext->clear();
}

// 以下函数保持不变...
void MainWindow::on_pushButton_8_clicked()
{
    if (!loginTimer->isActive()) {
        ui->stackedWidget->setCurrentIndex(1);
        usertext->clear();
    } else {
        page1test->setPlainText("登录失败，请等待倒计时结束！");
    }
}

void MainWindow::on_pushButton_9_clicked()               //  指纹     ---------------------------------------------------------->
{
    bool ret = Fingerprint_verification();
    if(ret)
    {
        if (loginTimer->isActive()) {
            loginTimer->stop();
            setLoginButtonsEnabled(true);
        }
        page1test->setPlainText(" 指纹验证成功 ！");
        ui->stackedWidget->setCurrentIndex(2);
        initPlainTextEdit();
    }else{
        page1test->setPlainText(" 指纹验证失败 ！");
    }
}




void MainWindow::on_pushButton_17_clicked()
{
    fingerprintCount++;
    fingerprintCount = fingerprintCount%6;
    ui->plainTextEdit_8->setPlainText(QString::number(fingerprintCount));
}



void MainWindow::on_pushButton_16_clicked()
{

    // 添加指纹
    QString oksql = sqlitedata->queryFingerprintLevel(ui->plainTextEdit_8->toPlainText().trimmed().toInt());
    if(oksql != "")
    {
        deviceAckShow->setPlainText("该 id 已录入 请重新选择 !!!");
        return;
    }

    sqlitedata->updateFingerprintLevel(ui->plainTextEdit_8->toPlainText().trimmed().toInt(),ui->plainTextEdit_9->toPlainText());

    bool ok=fingerPrint->inputAndStoreFingerPrint(ui->plainTextEdit_8->toPlainText().trimmed().toInt());
    if(!ok){
        cout << "录入指纹失败"<< endl;
    }else{
        cout << "录入指纹成功"<< endl;
    }

}

void MainWindow::on_pushButton_19_clicked()
{
    qos++;
    if(currentpermissions=="A")
    {
        qos %= 2; // 等价于 qos = qos%2
        ui->plainTextEdit_9->setPlainText(qos ? "B" : "A");
    }else{
        qos %= 3; // 0→S，1→A，2→B
        QString level = qos == 0 ? "S" : (qos == 1 ? "A" : "B");
        ui->plainTextEdit_9->setPlainText(level);
    }

}



void MainWindow::on_pushButton_20_clicked()  // 删除指纹
{
    if(currentpermissions.trimmed().toUpper() == "S")
    {
        bool ok = fingerPrint->deleteAllfingerprint();
        bool sqlok = sqlitedata ->deleteAllFingerprintData();
        if(ok && sqlok){
            cout << "删除All指纹成功"<< endl;
            deviceAckShow->setPlainText("删除All指纹成功");
        }else{
            cout << "删除All指纹失败"<< endl;
            deviceAckShow->setPlainText("删除All指纹失败");
        }

    }else{
        deviceAckShow->setPlainText("当前 用户 权限不足 !!!");
    }

}

void MainWindow::initPlainTextEdit()
{
    ui->plainTextEdit_5->setPlainText("doorlock");
    ui->plainTextEdit_6->setPlainText("0x01");
    ui->plainTextEdit_7->setPlainText("add");
    ui->plainTextEdit_8->setPlainText("1");
    ui->plainTextEdit_9->setPlainText("B");
}


bool MainWindow::Fingerprint_verification()
{
    page1test->setPlainText(" 正在进行指纹验证 ！");

    uint16_t id=0,score=0;
    bool flagbuffid=false;
    bool ok = fingerPrint->serchfingerprint(flagbuffid,id,score);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if(!ok){
        cout << "匹配失败"<< endl;
        return false;
    }else{
        if(flagbuffid)
        {
            cout << "--------------------------------------------------------!!!!!!!!!  指纹悬空  !!!!!!!!----"<< endl;
            return false;
        }else{
            cout << "-------------------------------------------------------------匹配指纹成功" << dec << " id: " << id << "   score: " << score << endl;
            currentpermissions = sqlitedata->queryFingerprintLevel(id);
            if(currentpermissions=="B")
            {
                deviceAckShow->setPlainText("当前 等级 过低 !!!");
                return false;
            }else if(currentpermissions=="A"){
                deviceAckShow->setPlainText("当前用户等级为 普通 用户 !!!");
            }else{
                deviceAckShow->setPlainText("当前用户等级为 高级 用户 !!!");
            }
            return true;
        }
    }


}

void MainWindow::on_pushButton_4_clicked()
{
    lasttext->insertPlainText("1");
}

void MainWindow::on_pushButton_5_clicked()
{
    lasttext->insertPlainText("2");
}

void MainWindow::on_pushButton_6_clicked()
{
    lasttext->insertPlainText("3");
}

void MainWindow::on_pushButton_7_clicked()
{
    lasttext->insertPlainText("4");
}

void MainWindow::on_pushButton_3_clicked()
{
    // 步骤1：先把光标移到文本末尾（确保删除的是最后一位，无论之前光标在哪）
    QTextCursor cursor = lasttext->textCursor();
    cursor.movePosition(QTextCursor::End);
    lasttext->setTextCursor(cursor);

    // 步骤2：获取当前文本，删除最后一位
    QString currentPwd = lasttext->toPlainText().trimmed(); // 避免空格干扰
    if (currentPwd.isEmpty()) return; // 文本为空时不操作

    // 截取前n-1位（删除最后一位）
    currentPwd = currentPwd.left(currentPwd.length() - 1);
    lasttext->setPlainText(currentPwd);

    // 步骤3：强制光标停在剩余文本的末尾（关键！确保后续输入在末尾追加）
    cursor = lasttext->textCursor();
    cursor.movePosition(QTextCursor::End);
    lasttext->setTextCursor(cursor);
}

QString intToHexString(int num) {
    num = qBound(0, num, 255);
    QString hexStr = QString::number(num, 16).toUpper();
    if (hexStr.length() < 2) hexStr = "0" + hexStr;
    return "0x" + hexStr;
}

QStringList deviceTypeList = {"doorlock", "sensor"}; // 可扩展更多类型
int deviceTypeListlen = deviceTypeList.size();

QStringList deviceNumList = {"0x01", "0x02","0x03", "0x04","0x05"}; // 可扩展更多类型
int deviceNumListlen = deviceNumList.size();

QStringList operationList = {"open", "close","add"}; // 可扩展更多类型
int operationListlen = operationList.size();

vector<uint8_t> MainWindow::getsenddata()
{
    QJsonObject jsondevice;
    jsondevice["deviceType"]=ui->plainTextEdit_6->toPlainText();
    jsondevice["deviceNumber"]=intToHexString(asd);
    asd++;

    QJsonObject jsonroot;
    jsonroot["operation"]=ui->plainTextEdit_5->toPlainText();
    jsonroot["command"]=ui->plainTextEdit_7->toPlainText();
    jsonroot["device"]=jsondevice;

    QJsonDocument jsd(jsonroot);
    QByteArray jsondata = jsd.toJson();
    std::vector<unsigned char> vec;
    // 转换：将 QByteArray 的数据复制到 vector
    vec.assign((unsigned char*)jsondata.data(), (unsigned char*)jsondata.data() + jsondata.size());

    return vec;
}


void MainWindow::on_pushButton_12_clicked()
{

    deviceTypeCount++;
    deviceTypeCount = deviceTypeCount%deviceTypeListlen;
    ui->plainTextEdit_5->setPlainText(deviceTypeList[deviceTypeCount]);
}


void MainWindow::on_pushButton_13_clicked()
{
    deviceNumCount++;
    deviceNumCount = deviceNumCount%deviceNumListlen;
    ui->plainTextEdit_6->setPlainText(deviceNumList[deviceNumCount]);
}


void MainWindow::on_pushButton_14_clicked()
{
    operationCount++;
    operationCount = operationCount%operationListlen;
    ui->plainTextEdit_7->setPlainText(operationList[operationCount]);
}


void MainWindow::receivestr()
{
    vector<uint8_t> recdata;
    bool ret = mqttdevice->receivesocket(recdata);
    if(!ret)
    {
        qDebug() << "receivesocket(recdata) failure";
        return;
    }
    deviceAckShow->clear();
    QByteArray jsonBytes(reinterpret_cast<const char*>(recdata.data()), recdata.size());
    QString jsonString = QString::fromUtf8(jsonBytes);

    // 添加JSON解析步骤和变量声明
    QJsonParseError parseError;  // 声明parseError变量
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);  // 解析JSON

    if (parseError.error == QJsonParseError::NoError) {
        // 成功解析JSON，格式化输出
        QString formattedJson = jsonDoc.toJson(QJsonDocument::Indented);
        qDebug() << "Received JSON data:";
        qDebug() << formattedJson;
        deviceAckShow->setPlainText(formattedJson);
        // 可以进一步处理JSON数据
        if(jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            // 处理JSON对象
        }
    } else {
        qDebug() << "JSON parse error:" << parseError.errorString();
        qDebug() << "Raw received data:" << jsonString;  // 输出原始数据以便调试
    }
}


void MainWindow::on_pushButton_10_clicked()
{
    vector<uint8_t> jsondata = getsenddata();
    bool ret = mqttdevice->sendsocket(jsondata);
    if(!ret)
    {
        qDebug() << "mqtt->sendsocket(jsondata) failure";
        return;
    }
    qDebug() << "mqtt->sendsocket(jsondata) success";

}

void MainWindow::onMessageReceived(const QString &jsonStr)
{
    deviceAckShow->clear();

    QJsonParseError parseError;  // 声明parseError变量
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);  // 解析JSON

    if (parseError.error == QJsonParseError::NoError) {
        // 成功解析JSON，格式化输出
        qDebug() << "JSON parse success";
        QString formattedJson = jsonDoc.toJson(QJsonDocument::Indented);
        qDebug() << "Received JSON data:";
        qDebug() << formattedJson;
        deviceAckShow->setPlainText(formattedJson);
        // 可以进一步处理JSON数据
        if(jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            // 处理JSON对象
        }

        QJsonObject jsonObj = jsonDoc.object();
        QJsonObject deviceObj = jsonObj["devicedata"].toObject();

        QJsonValue deviceNumJson = jsonObj["deviceNum"];
        int a = deviceNumJson.toInt(0);

        // 3. 去除errmsg首尾不可见字符，避免空字符串判断失效
        QString errmsg = deviceObj["errmsg"].toString().trimmed();
        if(errmsg=="")
        {
            sqlitedata->updateDeviceStatus(jsonObj["deviceAddr"].toString(),a,deviceObj["status"].toString());
            qDebug() << "add or updata success";
        }else{

            qDebug() << "add or updata failure";
        }

    } else {
        qDebug() << "JSON parse error:" << parseError.errorString();

    }
}

void MainWindow::onErrorOccurred(const QString &error)
{
    deviceAckShow->clear();
    qDebug() << "[MQTT接收错误]：" << error;
    deviceAckShow->setPlainText("JSON parse error");
}


void MainWindow::on_pushButton_11_clicked()
{
    deviceAckShow->clear();

    // 定义拼接字符串（用于格式化显示）
    QString showText;
    // 调用查询所有数据函数
    QList<QVariantMap> allData = sqlitedata->queryAllDeviceStatus();

    for (int i = 0; i < allData.size(); i++) {
        QVariantMap row = allData[i];

        // 拼接单条数据（格式可自定义，比如换行+分隔符）
        showText += QString("第 %1 条数据：\n")
                        .arg(i+1);
        showText += QString("  设备类型：%1\n")
                        .arg(row["deviceType"].toString());
        showText += QString("  设备编号：%1\n")  // 额外加16进制格式，可选
                        .arg(row["deviceNum"].toInt());
        showText += QString("  状态：%1\n")
                        .arg(row["status"].toString());
        showText += QString("  更新时间：%1\n\n")
                        .arg(row["updated_at"].toString());
    }

    // 无数据时提示
    if (allData.isEmpty()) {
        showText = "暂无设备状态数据！";
    }

    // 将拼接好的文本设置到UI控件
    deviceAckShow->setPlainText(showText);

    // 可选：设置文本只读，禁止用户编辑
    deviceAckShow->setReadOnly(true);
}


void MainWindow::on_pushButton_18_clicked()
{
    deviceAckShow->clear();

    // 定义拼接字符串（格式和设备数据保持一致）
    QString showText;
    // 调用指纹数据查询接口，获取所有指纹数据
    QList<QVariantMap> allFingerData = sqlitedata->queryAllFingerprintLevels();

    // 循环遍历所有指纹数据，按统一格式拼接
    for (int i = 0; i < allFingerData.size(); i++) {
        QVariantMap row = allFingerData[i];

        // 拼接单条指纹数据（格式和设备数据完全对齐）
        showText += QString("第 %1 条数据：\n")
                        .arg(i+1); // 从第1条开始计数，和设备数据一致
        showText += QString("  指纹ID：%1\n")
                        .arg(row["id"].toInt()); // 指纹ID（整数类型）
        showText += QString("  等级：%1\n")
                        .arg(row["level"].toString()); // 等级（S/A/B 字符串）
        showText += QString("  更新时间：%1\n\n")
                        .arg(row["updated_at"].toString()); // 最后更新时间
    }

    // 无指纹数据时的提示（和设备数据的无数据提示风格统一）
    if (allFingerData.isEmpty()) {
        showText = "暂无指纹等级数据！";
    }

    // 将拼接好的文本设置到UI控件
    deviceAckShow->setPlainText(showText);

    // 保持和设备数据一致：设置文本只读，禁止用户编辑
    deviceAckShow->setReadOnly(true);
}




