#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QGuiApplication"
#include "QScreen"
#include <QByteArray>
#include <QFile>
#include "ui_zhuye.h"
#include "ui_screen_3.h"
#include "ui_yunxing.h"
#include "ui_caxun.h"
#include <QDateTime>
#include <QListWidget>
#include <QtEndian>
#include <qmath.h>
#include <QWindow>

// 定义按钮样式常量：绿色按钮（普通状态）和红色按钮（按下状态）
const QString MainWindow::Greed_BUTTON_STYLE =
        "QPushButton {background: rgb(138, 226, 52);color: rgb(0, 0, 0);border-radius: 6px;}"
        "QPushButton:pressed {background: rgb(78, 154, 6);padding-top: 11px;padding-bottom: 9px;}";
const QString MainWindow::Red_BUTTON_STYLE =
        "QPushButton {background: rgb(239, 41, 41);color: rgb(0, 0, 0);border-radius: 6px;}"
        "QPushButton:pressed {background: rgb(78, 154, 6);padding-top: 11px;padding-bottom: 9px;}";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
#ifdef IMX6_PLATFORM
    // 若在嵌入式平台（IMX6）上，将窗口大小设置为第一个屏幕的大小
    QList<QScreen*> list_screen=QGuiApplication::screens();
    this->resize(list_screen.at(0)->geometry().width(),list_screen.at(0)->geometry().height());
#elif UBUNTU_PLATFORM
    // 设置一个合理的固定大小，或者也适应屏幕
    this->resize(1293, 835);  // 根据需要调整
#endif
    // 创建 Modbus 工作对象和线程，并移动到子线程中
    m_worker = &ModbusWorker::instance(slaveAddress);
    m_workerThread = new QThread(this);
    X1s_timer = new QTimer(this);        // 定时器：用于更新界面时间
    Receive_timer = new QTimer(this);     // 定时器：周期读取 Modbus 数据
    dlg_screen = new loginDlg(dbManager, this); // 登录界面

#ifdef IMX6_PLATFORM
    dbManager.openDatabase("/opt/Data_lib/ZongLin.db");
#elif UBUNTU_PLATFORM
    dbManager.openDatabase("/home/wp20/linux-imx6/XiangMu/ZongLin.db");
#endif
     // 创建数据库表
    dbManager.createTable_SheZhi();
    dbManager.createTable_jianche();
    dbManager.createTable_BiaoDing();
    dbManager.createTable_BaoJing();
    dbManager.createTable_CaoZuo();

    // 将 Modbus 工作对象移到子线程，并启动线程
    m_worker->moveToThread(m_workerThread);
//    Receive_timer->moveToThread(m_workerThread);
    connect(m_workerThread, &QThread::started, m_worker, &ModbusWorker::init_modbus_slot);  // 在线程启动后自动
    m_workerThread->start();

    // 创建所有界面并添加到 stackedWidget 中
    ZhuYe_screen=new Zhu_Ye_Screen(this);
    CaXun_screen=new Ca_Xun_Screen(this);
    GuanLi_screen=new screen_3(this);
    Yunxing_screen=new Yun_Xing_Screen(this);
    Tiaoshi_screen=new Tiao_Shi_Screen(this);
    SeZhi_screen = new Se_Zhi_Screen(this);
    Dengru_screen=new Deng_Lv_Screen(this);
    ui->stackedWidget->addWidget(dlg_screen);
    ui->stackedWidget->addWidget(ZhuYe_screen);
    ui->stackedWidget->addWidget(CaXun_screen);
    ui->stackedWidget->addWidget(GuanLi_screen);
    ui->stackedWidget->addWidget(SeZhi_screen);
    ui->stackedWidget->addWidget(Yunxing_screen);
    ui->stackedWidget->addWidget(Tiaoshi_screen);
    ui->stackedWidget->addWidget(Dengru_screen);
    ui->stackedWidget->setCurrentWidget(dlg_screen);// 默认显示登录界面

    ui->listWidget->hide();// 初始隐藏导航列表
    qApp->installEventFilter(this);// 安装事件过滤器，用于检测用户交互

    // 连接 ModbusWorker 的信号与主窗口的槽
    connect(m_worker,&ModbusWorker::errorOccurred,this,&MainWindow::InformPlayHandle);
    connect(m_worker,&ModbusWorker::WriteRegister_Inform,this,&MainWindow::InformPlayHandle);
    connect(m_worker,&ModbusWorker::DataReceived_16bit,this,&MainWindow::Modbus_handleData_16bit);
    connect(m_worker,&ModbusWorker::DataReceived_8bit,this,&MainWindow::Modbus_handleData_8bit);
    connect(m_worker,&ModbusWorker::connectionStatusChanged,GuanLi_screen,&screen_3::modbusstate_chang);
    connect(m_worker,&ModbusWorker::connectionStatusChanged,this,&MainWindow::modbusstate_chang);
     // 连接各子界面发出的 Modbus 操作信号到 ModbusWorker
    connect(ZhuYe_screen,&Zhu_Ye_Screen::WriteHoldRegisters_ZhuYe,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(GuanLi_screen,&screen_3::screen3_readHoldingRegisters,m_worker,&ModbusWorker::ReadHoldingRegisters);
    connect(GuanLi_screen,&screen_3::screen3_writeSingleRegister,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(SeZhi_screen,&Se_Zhi_Screen::WriteHoldRegisters_SeZhi,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(SeZhi_screen,&Se_Zhi_Screen::ReadHoldRegisters_SeZhi,m_worker,&ModbusWorker::ReadHoldingRegisters);
    connect(SeZhi_screen,&Se_Zhi_Screen::WriteHoldFloatRegisters_SeZhi,m_worker,&ModbusWorker::WriteHoldFloatRegister, Qt::BlockingQueuedConnection);
    connect(SeZhi_screen,&Se_Zhi_Screen::ReadInputRegisters_SeZhi,m_worker,&ModbusWorker::ReadInputRegisters);
    connect(SeZhi_screen,&Se_Zhi_Screen::WriteMultipleHoldRegisters_SeZhi,m_worker,&ModbusWorker::WriteMultipleHoldRegisters, Qt::BlockingQueuedConnection);
    connect(Tiaoshi_screen,&Tiao_Shi_Screen::WriteSingleCoil_TiaoShi,m_worker,&ModbusWorker::WriteSingleCoil, Qt::BlockingQueuedConnection);
    connect(Tiaoshi_screen,&Tiao_Shi_Screen::WriteHoldRegisters_TiaoShi,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(Tiaoshi_screen,&Tiao_Shi_Screen::ReadInputRegisters_TiaoShi,m_worker,&ModbusWorker::ReadInputRegisters);
    connect(Tiaoshi_screen,&Tiao_Shi_Screen::ReadHoldRegisters_TiaoShi,m_worker,&ModbusWorker::ReadHoldingRegisters);
    connect(Tiaoshi_screen,&Tiao_Shi_Screen::WriteHoldFloatRegisters_TiaoShi,m_worker,&ModbusWorker::WriteHoldFloatRegister, Qt::BlockingQueuedConnection);
    connect(Yunxing_screen,&Yun_Xing_Screen::WriteHoldFloatRegisters_YunXing,m_worker,&ModbusWorker::WriteHoldFloatRegister, Qt::BlockingQueuedConnection);
    connect(Yunxing_screen,&Yun_Xing_Screen::WriteHoldRegisters_YunXing,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(Yunxing_screen,&Yun_Xing_Screen::WriteSingleCoil_YunXing,m_worker,&ModbusWorker::WriteSingleCoil, Qt::BlockingQueuedConnection);
    connect(Yunxing_screen,&Yun_Xing_Screen::WriteMultipleCoils_YunXing,m_worker,&ModbusWorker::WriteMultipleCoils, Qt::BlockingQueuedConnection);
    connect(CaXun_screen,&Ca_Xun_Screen::WriteHoldFloatRegisters_CaXun,m_worker,&ModbusWorker::WriteHoldFloatRegister, Qt::BlockingQueuedConnection);
    connect(CaXun_screen,&Ca_Xun_Screen::WriteHoldRegisters_CaXun,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(CaXun_screen,&Ca_Xun_Screen::WriteSingleCoil_CaXun,m_worker,&ModbusWorker::WriteSingleCoil, Qt::BlockingQueuedConnection);


    // 连接定时器超时信号到槽函数
    connect(X1s_timer, &QTimer::timeout, this, &MainWindow::X1s_Timer_Handle);
    connect(Receive_timer,&QTimer::timeout,this,&MainWindow::Receive_timer_Handle);
    // 连接主窗口的 Modbus 操作信号到 ModbusWorker

    connect(this,&MainWindow::Read_Holding_Registers,m_worker,&ModbusWorker::ReadHoldingRegisters);
    connect(this,&MainWindow::Read_Input_Registers,m_worker,&ModbusWorker::ReadInputRegisters);
    connect(this,&MainWindow::Read_Bits_Registers,m_worker,&ModbusWorker::ReadBitsRegisters);
    connect(this,&MainWindow::Read_DiscreteInputs_Registers,m_worker,&ModbusWorker::ReadDiscreteInputs);
    connect(this,&MainWindow::WriteHoldRegisters,m_worker,&ModbusWorker::WriteHoldRegister, Qt::BlockingQueuedConnection);
    connect(this,&MainWindow::Trans_slaveaddress,m_worker,&ModbusWorker::Received_slaveaddress);
    connect(this,&MainWindow::modbus_reconnet,m_worker,&ModbusWorker::reconnect);
//    // 发送从站地址给 ModbusWorker--用于改变从机地址
//    emit Trans_slaveaddress(slaveAddress);
     // 初始化各种状态字符串列表（用于显示设备状态）
    DangQianZhuangTai<<"待启动"<<"水样测试"<<"标样核查"<<"零点核查"<<"跨度核查"<<"空白测试"<<"平行样测试"
    <<"加标回收"<<"定时空白标定"<<"定时量程标定"<<"初始化(清洗)"<<"停止测试"<<"仪器重启"<<"校时"<<"模式设置"
    <<"测量间隔设置"<<"零点核查间隔设置"<<"跨度核查间隔设置"<<"标样核查间隔设置"<<"手两标转维护之零标"<<"手两标转维护之量标"
    <<"手两标转水样之零标"<<"手两标转水样之量标"<<"维护测试"<<"零点标定"<<"量程标定"<<"C3标定"<<"C4标定"<<"C5标定"
    <<"空白校准"<<"标样校准"<<"核查不合格之零点校准"<<"核查不合格之量程校准"<<"手三标转维护之零标"<<"手三标转维护之标1"
    <<"手三标转维护之标2"<<"手三标转水样之零标"<<"手三标转水样之标1"<<"手三标定转水样之标2"<<"定时标1校准"<<"核查不合格之标1校准"
    <<"远程单次水样测试"<<"远程空白标定"<<"远程标样校准"<<"启动单台仪表标液核查"<<"定时标样核查"<<"外部触发水样";
    DangQianLiangCheng<<"未选择量程 0~ mg/L"<<"量程1 0~2mg/L"<<"量程2 0~10mg/L"<<"量程3 0~50mg/L";
    DangQianLiangCheng_len<<"未选择量程"<<"量程1"<<"量程2"<<"量程3";
    YuJingZhuangTai<<"正常"<<"采前液位信号出现异常"<<"试剂一余量不足"<<"试剂二余量不足"<<"试剂三余量不足"<<"试剂四余量不足"<<"试剂五余量不足"<<"蒸馏水余量不足"
                     <<"反应废液积量过满"<<"清洗废液积量过满"<<"蠕动泵转动信号预警"<<"光路信号异常";
    YunXingBuZhou<<"复位"<<"排空管路"<<"零点标定清洗计量管"<<"零点标定液位校准"<<"外接泵循环"<<"液位信号偏离清洗"<<"液位信号偏离校准"
    <<"润洗"<<"采样"<<"1次稀释"<<"2次稀释"<<"采蒸馏水"<<"采试剂一"<<"取比色电压V3"<<"采试剂二"<<"静置反应"<<"取比色电压V2"<<"排反应液"
    <<"清洗"<<"取比色电压V1"<<"计算"<<"加热消解"<<"冷却"<<"采试剂三"<<"进样后管路返回"<<"控温"<<"采标液"<<"采核查液"<<"排空管路-排比色皿"
    <<"计算吸光度"<<"等待采样信号"<<" "<<" "<<"已发定位命令"<<"已执行稀释定位"<<"上电清洗"<<"上电冷却"<<"上电操作完成"<<" "<<" "<<"超标报警"
    <<"跳转量程1"<<"跳转量程2"<<"跳转量程3"<<"跳转量程4"<<" "<<" "<<" "<<" "<<" "<<" "<<"标定结束"<<"等待下一次";
    JianCheMoShi<<"未选择模式"<<"连续模式"<<"周期模式"<<"定点模式"<<"受控模式"<<"手动模式";
    BaoJingZhuangTai<<"正常"<<"排空时液位信号异常"<<"润洗未采到样"<<"未采到样"<<"稀释1未采到稀释水"<<"稀释1未定到稀释水"<<"稀释1未采到稀释混合液"
    <<"稀释1定位异常"<<"稀释2未采到稀释水"<<"稀释2未定到稀释水"<<"稀释2未采到稀释混合液"<<"稀释2定位异常"<<"未采到采蒸馏水"<<"定蒸馏水异常"<<"未采到采试剂一"
    <<"定试剂一异常"<<"未采到采试剂二"<<"定试剂二异常"<<"未采到清洗水"<<"比色电压过低"<<"标样核查3次未通过"<<"检测次数已到"<<"定试样异常"<<"温度不能上升"
    <<"量程1曲线斜率异常"<<"量程2曲线斜率异常"<<"量程3曲线斜率异常"<<"量程4曲线斜率异常"<<"光谱仪信号过低"<<"扫描信号低于暗电流"<<"蠕动泵采样超时"
    <<"零标校准液位校准失败"<<"排液蠕动泵超时"<<"温度不能下降"<<"控温升温异常"<<"控温降温异常"<<"水样返回蠕动泵超时"<<"试剂返回蠕动泵超时"<<"抽比色皿蠕动泵超时"
    <<" "<<"量程信号低于零点信号"<<"未采到试剂三"<<"试剂三定位异常"<<"未采到标液"<<"标液定位异常"<<"未采到核查液"<<"核查液定位异常";

    DuoBuTiaoShi<<""<<"缺水样"<<"缺核查样"<<"缺量程样"<<"缺试剂一"<<"缺试剂二"<<"缺蒸馏水"<<"比色池无液体"<<"缺试剂三"<<"缺试剂四"<<"缺试剂五"<<"";
    ZhuangTai<<"禁用"<<"开启";
    ShuJuLeiXing<<"待启动"<<"水样数据"<<"标样数据"<<"零点核查"<<"跨度核查"<<""<<""<<"加标回收";
     // 启动定时器
    X1s_timer->start(5000);  // 每5秒更新一次时间
    screenchang_handle(0); // 默认切换到登录界面（索引0）;

}

MainWindow::~MainWindow()
{
    // 停止定时器
    if (X1s_timer) {
        X1s_timer->stop();
        delete X1s_timer;
        X1s_timer = nullptr;
    }
    if (Receive_timer) {
        Receive_timer->stop();
        delete Receive_timer;
        Receive_timer = nullptr;
    }

    // 停止子线程并等待结束
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait(3000); // 最多等3秒
        delete m_workerThread;
        m_workerThread = nullptr;
    }

//    // 删除Modbus工作对象（已移到子线程，线程停止后才安全删除）
//    if (m_worker) {
//        delete m_worker;
//        m_worker = nullptr;
//    }

    // 删除所有子界面
    delete ZhuYe_screen;     ZhuYe_screen = nullptr;
    delete CaXun_screen;     CaXun_screen = nullptr;
    delete GuanLi_screen;    GuanLi_screen = nullptr;
    delete SeZhi_screen;     SeZhi_screen = nullptr;
    delete Yunxing_screen;   Yunxing_screen = nullptr;
    delete Tiaoshi_screen;   Tiaoshi_screen = nullptr;
    delete Dengru_screen;    Dengru_screen = nullptr;
    delete dlg_screen;       dlg_screen = nullptr;

    delete ui;
}


/**
 * @brief 初始化数据读取：发送一系列读保持寄存器请求
 */
//void MainWindow::InitDataPlay()
//{
//    emit Read_Holding_Registers(7,1);//
//    emit Read_Holding_Registers(18,2);//
//    emit Read_Holding_Registers(40,1);//
//    emit Read_Holding_Registers(43,3);//
//    emit Read_Holding_Registers(54,2);//
//    emit Read_Holding_Registers(57,1);//
//    emit Read_Holding_Registers(62,1);//
//    emit Read_Holding_Registers(72,6);//
//    emit Read_Holding_Registers(98,8);//
//    emit Read_Holding_Registers(119,1);//
//    emit Read_Holding_Registers(121,1);//
//    emit Read_Holding_Registers(124,2);//
//    emit Read_Holding_Registers(146,3);//
//    emit Read_Holding_Registers(150,1);//
//    emit Read_Holding_Registers(155,8);//
//    emit Read_Holding_Registers(166,4);//
//    emit Read_Holding_Registers(178,3);//
//    emit Read_Holding_Registers(182,2);//
//    emit Read_Holding_Registers(188,2);//
//    emit Read_Holding_Registers(196,2);//
//    emit Read_Holding_Registers(206,2);//
//    emit Read_Holding_Registers(260,2);//
//    emit Read_Holding_Registers(270,2);//
//    emit Read_Holding_Registers(280,2);//

//    emit Read_Input_Registers(1,1);//
//}

/**
 * @brief 处理接收到的16位Modbus数据（保持寄存器、输入寄存器）
 * @param data 数据向量（16位值）
 * @param str  原始报文（十六进制字符串，用于校验）
 */
void MainWindow::Modbus_handleData_16bit(const QVector<uint16_t> &data,QString str)
{
    QByteArray frame  = QByteArray::fromHex(str.toLatin1());
     // 报文最小长度检查
    if ((str.length() < 12)||(frame.size() < 6))
    {
        InformPlayHandle("接受数据错误！");
        return;
    }
    quint8 Address = static_cast<quint8>(frame.at(0));
    quint8 functionCode = static_cast<quint8>(frame.at(1));
    quint16 startAddr = (static_cast<quint8>(frame.at(2)) << 8) | static_cast<quint8>(frame.at(3));
    quint16 numRegs = (static_cast<quint8>(frame.at(4)) << 8) | static_cast<quint8>(frame.at(5));
    if (data.size() < numRegs)
    {
        InformPlayHandle("接受数据长度小于给定值！");
        return;
    }
    if((Address==slaveAddress)&&(functionCode==0x03)) // 处理读保持寄存器（功能码0x03）
    {
            for(int i=0;i<numRegs;i++)
            {
                quint16 currentReg = startAddr + i;
                switch(currentReg) { // 根据寄存器地址将数据分发到对应界面的控件
                case (0):{this->CaXun_screen->ui->label_70->setText(QString::number(data[i]));break;}
                case (5):{this->SeZhi_screen->ui->Label_29->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_82->setText(QString::number(data[i]));break;}
                case (6):{this->SeZhi_screen->ui->Label_30->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_84->setText(QString::number(data[i]));break;}
                case (7):{this->SeZhi_screen->ui->LineEdit_3->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_85->setText(QString::number(data[i]));break;}
                case (8):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    memcpy(&Biaodingnongdu, &littleEndianValue , sizeof(float));i++; break;}
                case (10):{if(data[i]==1){ WriteHoldRegisters(10,0);QMap<QString, QVariant> setData;setData["bdlx"] = Biaodingleixing;
                        setData["lc"] =this->ZhuYe_screen->ui->label_18->text();setData["nd"] = QString::number(Biaodingnongdu);
                        setData["bdxgd"] = QString::number(Xiguangdu);setData["ecx"] ="0";setData["ycx"] = QString::number(Yichixiang);
                        setData["csx"] = QString::number(Changshuxiang);dbManager.insertRecord("[biaoding]", setData);}break;}
                //读取单片机时钟检查时间是否设置成功
                case (12):{if(startAddr == 12 && numRegs == 6 && data.size() >= 6){
                        QDateTime slaveTime(QDate(data[0], data[1], data[2]),QTime(data[3], data[4], data[5]));
                        QDateTime currentDateTime = QDateTime::currentDateTime();qint64 diffSeconds = qAbs(slaveTime.secsTo(currentDateTime));
                        if(diffSeconds <= 10){InformPlayHandle("时间设置成功!");}else{InformPlayHandle("时间设置失败!");}}break;}

                case (18):{this->Yunxing_screen->ui->lineEdit_28->setText(QString::number(data[i]));break;}
                case (19):{this->Tiaoshi_screen->ui->LineEdit->setText(QString::number(data[i]));break;}
                case (40):{this->SeZhi_screen->ui->LineEdit_15->setText(QString::number(data[i]));
                    this->Tiaoshi_screen->ui->lineEdit_5->setText(QString::number(data[i]));break;}
                case (41):{this->Tiaoshi_screen->ui->lineEdit_2->setText(QString::number(data[i]));break;}
                case (42):{this->Tiaoshi_screen->ui->lineEdit_4->setText(QString::number(data[i]));break;}
                case (43):{this->SeZhi_screen->ui->LineEdit_13->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_121->setText(QString::number(data[i]));break;}
                case (44):{this->SeZhi_screen->ui->LineEdit_12->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_120->setText(QString::number(data[i]));break;}
                case (45):{this->SeZhi_screen->ui->LineEdit_14->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_114->setText(QString::number(data[i]));break;}
                case (54):{this->Tiaoshi_screen->ui->LineEdit_2->setText(QString::number(data[i]));break;}
                case (55):{this->Tiaoshi_screen->ui->LineEdit_6->setText(QString::number(data[i]));break;}
                case (57):{this->Tiaoshi_screen->ui->lineEdit_6->setText(QString::number(data[i]));break;}
                case (62):{this->SeZhi_screen->ui->LineEdit_16->setText(QString::number(data[i]));break;}
                case (63):{this->ZhuYe_screen->ui->label_21->setText(QString::number(data[i]));break;}
                case (66):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->label_53->setText(QString::number(floatValue,'f',3));
                    this->Tiaoshi_screen->ui->label_48->setText(QString::number(floatValue,'f',3));
                    this->CaXun_screen->ui->label_79->setText(QString::number(floatValue,'f',3));i++; break;}
                case (72):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->LineEdit_3->setText(QString::number(floatValue,'f',3));i++; break;}
                case (74):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->LineEdit_4->setText(QString::number(floatValue,'f',3));i++; break;}
                case (76):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->LineEdit_5->setText(QString::number(floatValue,'f',3));i++; break;}
                case (98):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->lineEdit_7->setText(QString::number(floatValue,'f',3));i++; break;}
                case (100):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->lineEdit->setText(QString::number(floatValue,'f',3));i++; break;}
                case (102):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Tiaoshi_screen->ui->lineEdit_3->setText(QString::number(floatValue,'f',3));i++; break;}
                case (104):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->ui->lineEdit_15->setText(QString::number(floatValue,'f',3));
                    this->Tiaoshi_screen->ui->lineEdit_8->setText(QString::number(floatValue,'f',3));i++; break;}
                case (108):{if(data[i]<=47){this->ZhuYe_screen->ui->label_14->setText(DangQianZhuangTai.at(data[i]));
                    this->CaXun_screen->ui->label_71->setText(DangQianZhuangTai.at(data[i]));}else{InformPlayHandle("当前状态类型错误！");}
                    if(data[i]==8||data[i]==19||data[i]==21||data[i]==24||data[i]==29||data[i]==33||data[i]==36||data[i]==41){Biaodingleixing="零标";}
                    else if(data[i]==9||data[i]==20||data[i]==22||data[i]==25||data[i]==30||data[i]==32||data[i]==34||data[i]==37||data[i]==42){Biaodingleixing="量标";}
                    else{Biaodingleixing="无";}break;}
                case (109):{this->SeZhi_screen->ui->LineEdit_11->setText(QString::number(data[i]));break;}
                case (110):{this->SeZhi_screen->ui->LineEdit_10->setText(QString::number(data[i]));break;}
                case (116):{/*this->CaXun_screen->ui->label_85->setText(QString::number(data[i]));*/
                    if(data[i]<=11){this->ZhuYe_screen->ui->label_20->setText(YuJingZhuangTai.at(data[i]));}
                    else{InformPlayHandle("预警状态类型错误！");}break;}
                case (117):{if(data[i]<=52){this->ZhuYe_screen->ui->label_15->setText(YunXingBuZhou.at(data[i]));this->CaXun_screen->ui->label_72->setText(YunXingBuZhou.at(data[i]));}
                        else if(data[i]<=98 || data[i]==100){this->ZhuYe_screen->ui->label_15->setText(" ");this->CaXun_screen->ui->label_72->setText(" ");}
                        else if(data[i]==99){this->ZhuYe_screen->ui->label_15->setText("报警停机");this->CaXun_screen->ui->label_72->setText("报警停机");}
                        else{InformPlayHandle("运行步骤类型错误！"); }
                        if(Caozuorizhi!=data[i] && (data[i]<=52 || data[i]==99)){Caozuorizhi=data[i];
                            QMap<QString, QVariant> setData;setData["czrz"] = this->ZhuYe_screen->ui->label_15->text();
                            dbManager.insertRecord("[caozuo]", setData);}break;}
                case (118):{if(data[i]<=46){this->ZhuYe_screen->ui->label_16->setText(BaoJingZhuangTai.at(data[i]));
                           if(Rizhibaojing!=data[i]){Rizhibaojing=data[i];QMap<QString, QVariant> setData;setData["nr"] = BaoJingZhuangTai.at(data[i]);
                             dbManager.insertRecord("[baojing]", setData);}}
                            else if(data[i]<=100){this->ZhuYe_screen->ui->label_16->setText(" ");}
                            else{InformPlayHandle("报警状态类型错误！");}break;}

                case (119):{this->SeZhi_screen->ui->LineEdit_4->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_89->setText(QString::number(data[i]));break;}
                case (121):{this->SeZhi_screen->ui->LineEdit_7->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_83->setText(QString::number(data[i]));break;}
                case (124):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->ui->LineEdit_17->setText(QString::number(floatValue,'f',3));i++;break;}
                case (133):{if(data[i]<=3){this->ZhuYe_screen->ui->label_18->setText(DangQianLiangCheng.at(data[i]));
                        this->Yunxing_screen->LC_Label5[0]->setText(QString::number(data[i]));
                        this->CaXun_screen->ui->label_69->setText(DangQianLiangCheng_len.at(data[i]));}else{InformPlayHandle("当前量程类型错误！");}break;}
                case (134):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    memcpy(&Xiguangdu, &littleEndianValue , sizeof(float));i++;break;}
                case (136):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    memcpy(&Yichixiang, &littleEndianValue , sizeof(float));i++;break;}
                case (138):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    memcpy(&Changshuxiang, &littleEndianValue , sizeof(float));i++;break;}
                case (140):{if((data[i]==1)&&(Hold140==false)){WriteHoldRegisters(140,0);
                        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                        this->ZhuYe_screen->ui->label_17->setText(currentTime);QMap<QString, QVariant> setData;
                        setData["v1"] = this->CaXun_screen->ui->label_124->text();setData["v2"] = this->CaXun_screen->ui->label_127->text();
                        setData["value1"] = this->ZhuYe_screen->ui->label_2->text();setData["sylc"] = this->ZhuYe_screen->ui->label_18->text();
                        setData["sjlx"] = this->ZhuYe_screen->ui->label_13->text();dbManager.insertRecord("[jianche]", setData);
                        Hold140 = true;QTimer::singleShot(10000, this, [this]() {Hold140 = false;});}
                        else if((data[i]==0)&&(Hold140 ==true)){Hold140 = false;}break;}
                case (144):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                           float floatValue;floatValue=Uint16CoverFloat(data[i],data[i+1]);
                            this->ZhuYe_screen->ui->label_2->setText(QString::number(floatValue,'f',3));i++;break;}
                case (146):{this->SeZhi_screen->SinJiTiJi_line[1][0]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[1][0]->setText(QString::number(data[i]));break;}
                case (147):{this->SeZhi_screen->SinJiTiJi_line[1][1]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[1][1]->setText(QString::number(data[i]));break;}
                case (148):{this->SeZhi_screen->SinJiTiJi_line[1][2]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[1][2]->setText(QString::number(data[i]));break;}
                case (149):{if(data[i]<=5){this->ZhuYe_screen->ui->label_19->setText(JianCheMoShi.at(data[i]));
                            this->Yunxing_screen->ui->label_52->setText(JianCheMoShi.at(data[i]));
                            this->CaXun_screen->ui->label_74->setText(JianCheMoShi.at(data[i]));}
                            else{InformPlayHandle("模式读取类型错误!");}break;}
                case (150):{this->Yunxing_screen->ui->lineEdit_63->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_76->setText(QString::number(data[i]));break;}
                case (155):{this->SeZhi_screen->SinJiTiJi_line[1][3]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[1][3]->setText(QString::number(data[i]));break;}
                case (156):{this->SeZhi_screen->SinJiTiJi_line[0][0]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[0][0]->setText(QString::number(data[i]));break;}
                case (157):{this->SeZhi_screen->SinJiTiJi_line[0][1]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[0][1]->setText(QString::number(data[i]));break;}
                case (158):{this->SeZhi_screen->SinJiTiJi_line[0][2]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[0][2]->setText(QString::number(data[i]));break;}
                case (159):{this->SeZhi_screen->SinJiTiJi_line[0][3]->setText(QString::number(data[i]));
                    this->CaXun_screen->SinJiTiJi_line[0][3]->setText(QString::number(data[i]));break;}
                case (160):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->ui->lineEdit_13->setText(QString::number(floatValue,'f',3));i++;break;}
                case (162):{this->SeZhi_screen->ui->lineEdit_14->setText(QString::number(data[i]));break;}
                case (166):{this->Yunxing_screen->ui->lineEdit_53->setText(QString::number(data[i]));break;}
                case (167):{this->Yunxing_screen->ui->lineEdit_54->setText(QString::number(data[i]));break;}
                case (168):{if(data[i]<=1){SetPushButtonText(data[i],this->Yunxing_screen->ui->pushButton_128,ZhuangTai);
                    SetPushButtonText(data[i],this->Yunxing_screen->ui->pushButton_129,ZhuangTai);}break;}
                case (169):{this->Yunxing_screen->ui->lineEdit_55->setText(QString::number(data[i]));break;}
                case (170):{this->Yunxing_screen->ui->lineEdit_56->setText(QString::number(data[i]));break;}
                case (171):{if(data[i]<=1){SetPushButtonText(data[i],this->Yunxing_screen->ui->pushButton_130,ZhuangTai);}break;}
                case (178):{this->Yunxing_screen->ui->lineEdit_50->setText(QString::number(data[i]));break;}
                case (179):{this->Yunxing_screen->ui->lineEdit_52->setText(QString::number(data[i]));break;}
                case (180):{this->Yunxing_screen->ui->lineEdit_51->setText(QString::number(data[i]));break;}
                case (182):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->ui->lineEdit_49->setText(QString::number(floatValue,'f',3));i++; break;}
//                case (185):{if(data[i]<=10){this->Tiaoshi_screen->ui->label_12->setText(DuoBuTiaoShi.at(data[i]));}
//                        else{InformPlayHandle("多步调试类型错误！");}break;}
                case (188):{this->Yunxing_screen->ui->lineEdit_58->setText(QString::number(data[i]));break;}
                case (189):{this->Yunxing_screen->ui->lineEdit_57->setText(QString::number(data[i]));break;}
                case (190):{this->Yunxing_screen->ui->lineEdit_59->setText(QString::number(data[i]));break;}
                case (194):{if(data[i]<=1){SetPushButtonText(data[i],this->Yunxing_screen->ui->pushButton_131,ZhuangTai);}break;}
                case (195):{if(data[i]<=1){SetPushButtonText(data[i],this->Yunxing_screen->ui->pushButton_132,ZhuangTai);}break;}
                case (196):{this->Yunxing_screen->ui->lineEdit_62->setText(QString::number(data[i]));break;}
                case (197):{this->Yunxing_screen->ui->lineEdit_61->setText(QString::number(data[i]));break;}
                case (198):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->ui->lineEdit_65->setText(QString::number(floatValue,'f',3));i++; break;}
                case (202):{this->Yunxing_screen->ui->lineEdit_66->setText(QString::number(data[i]));break;}
                case (204):{if(data[i]<=1){this->Yunxing_screen->ui->checkBox->setText(ZhuangTai.at(data[i]));}break;}
                case (206):{this->SeZhi_screen->ui->LineEdit_8->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_91->setText(QString::number(data[i]));break;}
                case (207):{this->Yunxing_screen->ui->lineEdit_64->setText(QString::number(data[i]));
                    this->CaXun_screen->ui->label_75->setText(QString::number(data[i]));break;}
                case (208):{this->Yunxing_screen->LC_Label1[0]->setText(QString::number(data[i]));
                    this->Yunxing_screen->LC_Label2[0]->setText(QString::number(data[i]));
                    this->Yunxing_screen->LC_Label3[0]->setText(QString::number(data[i]));break;}
                case (209):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_line1->setText(QString::number(floatValue,'f',3));i++; break;}
                case (211):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_line2->setText(QString::number(floatValue,'f',3));i++; break;}
                case (213):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_line3->setText(QString::number(floatValue,'f',3));i++; break;} 
                default:break;
                }
            }
    }
    else if((Address==slaveAddress)&&(functionCode==0x04)){// 处理读输入寄存器（功能码0x04）
        for(int i=0;i<numRegs;i++){
            quint16 currentReg = startAddr + i;
            switch(currentReg){
            case (1):{this->CaXun_screen->ui->label_116->setText(QString::number(data[i]));break;}
            case (7):{this->Tiaoshi_screen->ui->label_54->setText(QString::number(data[i]));break;}
            case (8):{this->CaXun_screen->ui->label_68->setText(QString::number(static_cast<int16_t>(data[i])));
                if(static_cast<int16_t>(data[i])<0){SetPushButton(true,this->CaXun_screen->ui->pushButton_30);
                SetPushButton(false,this->CaXun_screen->ui->pushButton_27);}
                else if(static_cast<int16_t>(data[i])>0){SetPushButton(false,this->CaXun_screen->ui->pushButton_30);
                    SetPushButton(true,this->CaXun_screen->ui->pushButton_27);}break;}
            case (13):{this->Tiaoshi_screen->ui->label_36->setText(QString::number(data[i]));
                this->Tiaoshi_screen->ui->label_47->setText(QString::number(data[i]));
                 this->CaXun_screen->ui->label_80->setText(QString::number(data[i]));break;}
            case (14):{this->Tiaoshi_screen->ui->label_44->setText(QString::number(data[i]));
                this->Tiaoshi_screen->ui->label_23->setText(QString::number(data[i]));
                this->Tiaoshi_screen->ui->label_34->setText(QString::number(data[i]));
                this->CaXun_screen->ui->label_77->setText(QString::number(data[i]));break;}
            case (15):{this->Tiaoshi_screen->ui->label_45->setText(QString::number(data[i]));
                this->Tiaoshi_screen->ui->label_46->setText(QString::number(data[i]));
                this->Tiaoshi_screen->ui->label_31->setText(QString::number(data[i]));
                this->CaXun_screen->ui->label_78->setText(QString::number(data[i]));
                this->CaXun_screen->ui->label_86->setText(QString::number(data[i]));
                this->CaXun_screen->ui->label_117->setText(QString::number(data[i]));break;}
            case (24):{this->CaXun_screen->ui->label_127->setText(QString::number(data[i]));break;}
            case (25):{this->CaXun_screen->ui->label_124->setText(QString::number(data[i]));break;}
//            case (43):{if(startAddr == 38 && numRegs == 6 && data.size() >= 6){
//                    QDateTime slaveTime(QDate(data[0], data[1], data[2]),QTime(data[3], data[4], data[5]));
//                    QString dateTimeString = slaveTime.toString("yyyy-MM-dd HH:mm:ss");
//                    this->ZhuYe_screen->ui->label_17->setText(dateTimeString);}
//                    else{InformPlayHandle("数据时间读取失败!");}break;}
            case (54):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->CaXun_screen->ui->label_115->setText(QString::number(floatValue,'f',3));i++;break;}
            case (56):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->SinJiTiJi_line[2][0]->setText(QString::number(floatValue,'f',3));
                    this->CaXun_screen->SinJiTiJi_line[2][0]->setText(QString::number(floatValue,'f',3));i++;;break;}
            case (58):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->SinJiTiJi_line[2][1]->setText(QString::number(floatValue,'f',3));
                    this->CaXun_screen->SinJiTiJi_line[2][1]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (60):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->SinJiTiJi_line[2][2]->setText(QString::number(floatValue,'f',3));
                    this->CaXun_screen->SinJiTiJi_line[2][2]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (62):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->SeZhi_screen->SinJiTiJi_line[2][3]->setText(QString::number(floatValue,'f',3));
                    this->CaXun_screen->SinJiTiJi_line[2][3]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (64):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->ui->label_10->setText(QString::number(floatValue,'f',3));i++;break;}
            case (66):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue, sizeof(float));
                    if(floatValue>=0){this->Yunxing_screen->ui->label_80->setText("+"+QString::number(floatValue,'f',3)); }
                    else{this->Yunxing_screen->ui->label_80->setText(QString::number(floatValue,'f',3));}
                    this->Yunxing_screen->LC_Label5[4]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (68):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    if(floatValue>=0){this->Yunxing_screen->ui->label_98->setText("+"+QString::number(floatValue,'f',3)); }
                    else{this->Yunxing_screen->ui->label_98->setText(QString::number(floatValue,'f',3));}
                    this->Yunxing_screen->LC_Label5[7]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (70):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_Label1[1]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (72):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_Label1[2]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (74):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->ui->label_12->setText(QString::number(floatValue,'f',3));i++;break;}
            case (76):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    if(floatValue>=0){this->Yunxing_screen->ui->label_99->setText("+"+QString::number(floatValue,'f',3)); }
                    else{this->Yunxing_screen->ui->label_99->setText(QString::number(floatValue,'f',3));}
                    this->Yunxing_screen->LC_Label5[5]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (78):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    if(floatValue>=0){this->Yunxing_screen->ui->label_100->setText("+"+QString::number(floatValue,'f',3)); }
                    else{this->Yunxing_screen->ui->label_100->setText(QString::number(floatValue,'f',3));}
                    this->Yunxing_screen->LC_Label5[8]->setText(QString::number(floatValue,'f',3));i++;break;}

            case (86):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                   quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    if(floatValue>=0){this->Yunxing_screen->ui->label_102->setText("+"+QString::number(floatValue,'f',3)); }
                    else{this->Yunxing_screen->ui->label_102->setText(QString::number(floatValue,'f',3));} i++;break;}
            case (88):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    if(floatValue>=0){this->Yunxing_screen->ui->label_103->setText("+"+QString::number(floatValue,'f',3)); }
                    else{this->Yunxing_screen->ui->label_103->setText(QString::number(floatValue,'f',3));}
                    this->Yunxing_screen->LC_Label5[6]->setText(QString::number(floatValue,'f',3));
                    this->Yunxing_screen->LC_Label5[9]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (94):{if((data[i]==0)||(data[i]==1)||(data[i]==2)||(data[i]==3)||(data[i]==4)||(data[i]==7))
                {this->ZhuYe_screen->ui->label_13->setText(ShuJuLeiXing.at(data[i]));}
                else if(data[i]==23){this->ZhuYe_screen->ui->label_13->setText("维护测试");}
                else if(data[i]==45){this->ZhuYe_screen->ui->label_13->setText("外部触发");}break;}

            case (80):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_Label2[1]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (82):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_Label2[2]->setText(QString::number(floatValue,'f',3));i++;break;}

            case (90):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_Label3[1]->setText(QString::number(floatValue,'f',3));i++;break;}
            case (92):{if (i + 1 >= data.size()){InformPlayHandle("数据长度不足，无法读取浮点数");break;}
                    quint32 littleEndianValue  = (static_cast<quint32>(data[i+1]) << 16) | data[i];
                    float floatValue;memcpy(&floatValue, &littleEndianValue , sizeof(float));
                    this->Yunxing_screen->LC_Label3[2]->setText(QString::number(floatValue,'f',3));i++;break;}

            default:break;
          }
       }
    }
}
/**
 * @brief 处理接收到的8位Modbus数据（线圈、离散输入）
 * @param data 数据向量（8位值）
 * @param str  原始报文（十六进制字符串，用于校验）
 */
void MainWindow::Modbus_handleData_8bit(const QVector<uint8_t> &data, QString str)
{
    QByteArray frame  = QByteArray::fromHex(str.toLatin1());
    if ((str.length() < 12)||(frame.size() < 6))
    {
        InformPlayHandle("接受数据错误！");
        return;
    }
    quint8 slaveAddress = static_cast<quint8>(frame.at(0));
    quint8 functionCode = static_cast<quint8>(frame.at(1));
    quint16 startAddr = (static_cast<quint8>(frame.at(2)) << 8) | static_cast<quint8>(frame.at(3));
    quint16 numRegs = (static_cast<quint8>(frame.at(4)) << 8) | static_cast<quint8>(frame.at(5));
    if (data.size() < numRegs)
    {
        InformPlayHandle("接受数据长度小于给定值！");
        return;
    }
    if((slaveAddress==0x01)&&(functionCode==0x01)) // 处理读线圈（功能码0x01）
    {
        for(int i=0;i<numRegs;i++)
        {
             quint16 currentReg = startAddr + i;
             bool coilValue=(data[i]!=0);
             switch(currentReg)
             {
             case(0):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_10);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_20);break;}
             case(1):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_12);
                 SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_19);break;}
             case(2):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_7);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_23);break;}
             case(3):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_16);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_33);break;}
             case(4):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_19);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_29);break;}
             case(5):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_8);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_32);break;}
             case(6):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_9);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_37);break;}
             case(7):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_13);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_24);break;}
             case(8):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_17);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_28);break;}
             case(9):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_20);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_25);break;}
             case(10):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_21);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_31);break;}
             case(11):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_18);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_26);break;}
             case(12):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_11);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_34);break;}
             case(13):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_14);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_18);break;}
             case(14):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_15);
                  SetPushButton(coilValue,this->CaXun_screen->ui->pushButton_21);break;}
             case(15):{SetPushButton(coilValue,this->Tiaoshi_screen->ui->pushButton_22);break;}
             case(40):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_116);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_23);break;}
             case(41):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_110);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_22);break;}
             case(42):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_120);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_21);break;}
             case(43):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_100);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_20);break;}
             case(44):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_115);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_19);break;}
             case(45):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_108);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_18);break;}
             case(46):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_123);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_17);break;}
             case(47):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_114);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_16);break;}
             case(48):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_121);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_15);break;}
             case(49):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_113);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_14);break;}
             case(50):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_107);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_13);break;}
             case(51):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_118);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_12);break;}
             case(52):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_122);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_11);break;}
             case(53):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_102);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_10);break;}
             case(54):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_106);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_9);break;}
             case(55):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_119);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_8);break;}
             case(56):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_117);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_7);break;}
             case(57):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_112);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_6);break;}
             case(58):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_109);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_5);break;}
             case(59):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_111);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_4);break;}
             case(60):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_105);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_3);break;}
             case(61):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_104);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_2);break;}
             case(62):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_101);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_1);break;}
             case(63):{SetPushButton(coilValue,this->Yunxing_screen->ui->pushButton_103);
                 SetQCheckBox(coilValue,this->CaXun_screen->ui->checkBox_0);break;}
             case(151):{if(coilValue){this->Tiaoshi_screen->ui->radioButton_3->setChecked(true);}else{this->Tiaoshi_screen->ui->radioButton_3->setChecked(false);}break;}
             case(152):{if(coilValue){this->Tiaoshi_screen->ui->radioButton_4->setChecked(true);}else{this->Tiaoshi_screen->ui->radioButton_4->setChecked(false);}break;}
             default:break;
             }
        }
    }
    if((slaveAddress==0x01)&&(functionCode==0x02))// 处理读离散输入（功能码0x02）
    {
        for(int i=0;i<numRegs;i++)
        {
             quint16 currentReg = startAddr + i;
             bool coilValue=(data[i]!=0);
             switch(currentReg)
             {
             case(0):{if(coilValue){this->Tiaoshi_screen->ui->radioButton->setChecked(true);}else{this->Tiaoshi_screen->ui->radioButton->setChecked(false);}break;}
             case(1):{if(coilValue){this->Tiaoshi_screen->ui->radioButton_2->setChecked(true);}else{this->Tiaoshi_screen->ui->radioButton_2->setChecked(false);}break;}
             default:break;
             }
        }
    }
}
/**
 * @brief 设置按钮的样式和属性，反映线圈状态
 * @param state     线圈状态（true=ON，false=OFF）
 * @param pushbotton 目标按钮
 */
void MainWindow::SetPushButton(bool state,QPushButton *pushbotton)
{
    if(state){
        pushbotton->setStyleSheet(Red_BUTTON_STYLE);
        pushbotton->setProperty("state","ON");
    }
    else{
        pushbotton->setStyleSheet(Greed_BUTTON_STYLE);
        pushbotton->setProperty("state","OFF");
    }
}

void MainWindow::SetQCheckBox(bool state, QCheckBox* cheackbox)
{
    if(state){cheackbox->setChecked(true);}
    else{cheackbox->setChecked(false);}
}

void MainWindow::SetPushButtonText(int state, QPushButton *pushbotton,QStringList ZhuangTai)
{
    if(state<=1){
        if(state==0){
            pushbotton->setStyleSheet(Greed_BUTTON_STYLE);
            pushbotton->setProperty("state","OFF");
        }
        else if(state==1){
            pushbotton->setStyleSheet(Red_BUTTON_STYLE);
            pushbotton->setProperty("state","ON");
        }
        pushbotton->setText(ZhuangTai.at(state));
    }
    else{InformPlayHandle("数据读取错误!");}
}

/**
 * @brief 判断事件是否为用户交互事件（鼠标、键盘、触摸）
 * @param event 事件对象
 * @return 如果是交互事件返回true，否则false
 */
bool MainWindow::isUserInteractionEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::TouchBegin:
    case QEvent::KeyPress:
        return true;
    default:
        return false;
    }
}

float MainWindow::Uint16CoverFloat(uint16_t Low, uint16_t Hig)
{
    float floatValue;
    quint32 littleEndianValue  = (static_cast<quint32>(Hig) << 16) | Low;
    memcpy(&floatValue, &littleEndianValue , sizeof(float));
    return floatValue;
}
/**
 * @brief 切换主界面（由导航栏触发）
 * @param gotoscreen 目标界面索引（0~7）
 */
void MainWindow::screenchang_handle(int gotoscreen)
{
    if(gotoscreen==0)
    {
        ui->stackedWidget->setCurrentWidget(dlg_screen);
        ui->listWidget->hide();
    }
    else if(gotoscreen==1)
    {
        ui->stackedWidget->setCurrentWidget(ZhuYe_screen);
        ui->listWidget->show();
    }
    else if(gotoscreen==2)
    {
        if((YH_QuanXian==1)||(YH_QuanXian==0))
        {
            ui->stackedWidget->setCurrentWidget(CaXun_screen);
            ui->listWidget->show();
        }
        else if(YH_QuanXian==2)
        {
            InformPlayHandle("用户权限错误！");
        }
    }   
    else if(gotoscreen==3)
    {
        if((YH_QuanXian==1)||(YH_QuanXian==0))
        {
            ui->stackedWidget->setCurrentWidget(SeZhi_screen);
            ui->listWidget->show();
        }
        else if(YH_QuanXian==2)
        {
            InformPlayHandle("用户权限错误！");
        }
    }
    else if(gotoscreen==4)
    {
        if((YH_QuanXian==1)||(YH_QuanXian==0))
        {
            ui->stackedWidget->setCurrentWidget(Tiaoshi_screen);
            ui->listWidget->show();
        }
        else if(YH_QuanXian==2)
        {
            InformPlayHandle("用户权限错误！");
        }
    }
    else if(gotoscreen==5)
    {
        if((YH_QuanXian==1)||(YH_QuanXian==0))
        {
            ui->stackedWidget->setCurrentWidget(Yunxing_screen);
            ui->listWidget->show();
        }
        else if(YH_QuanXian==2)
        {
            InformPlayHandle("用户权限错误！");
        }
    }
    else if(gotoscreen==6)
    {
        if((YH_QuanXian==1)||(YH_QuanXian==0))
        {
            ui->stackedWidget->setCurrentWidget(GuanLi_screen);
            ui->listWidget->show();
        }
        else if(YH_QuanXian==2)
        {
            InformPlayHandle("用户权限错误！");
        }
    }
    else if(gotoscreen==7)
    {
        ui->stackedWidget->setCurrentWidget(Dengru_screen);
        ui->listWidget->show();
    }
}
/**
 * @brief 每秒定时器槽函数：更新界面时间显示
 */
void MainWindow::X1s_Timer_Handle()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeString = currentDateTime.toString("yyyy-MM-dd HH:mm");
    ui->label_2->setText(dateTimeString);
    if(modbus_static==false){
        InformPlayHandle("从机正在连接......");
        emit modbus_reconnet();
    }
}
/**
 * @brief 每2秒定时器槽函数：批量读取 Modbus 数据，刷新所有界面
 */
void MainWindow::Receive_timer_Handle()
{
    Receive_timer->stop();
    emit Read_Holding_Registers(0,1);//
    emit Read_Holding_Registers(5,6);//
    emit Read_Holding_Registers(18,2);//
    emit Read_Holding_Registers(40,1);//
    emit Read_Holding_Registers(43,3);//
    emit Read_Holding_Registers(54,2);//
    emit Read_Holding_Registers(57,1);//
    emit Read_Holding_Registers(62,2);//

    QTimer::singleShot(150, this, [this]{
        emit Read_Holding_Registers(66,2);//
        emit Read_Holding_Registers(72,6);//
        emit Read_Holding_Registers(98,8);//
        emit Read_Holding_Registers(108,3);//
        emit Read_Holding_Registers(116,4);//
        emit Read_Input_Registers(24,2);//注意
        emit Read_Holding_Registers(144,7);//注意
      });

    QTimer::singleShot(300, this, [this]{
        emit Read_Holding_Registers(121,1);//
        emit Read_Holding_Registers(124,2);//
        emit Read_Holding_Registers(133,7);//   
        emit Read_Holding_Registers(155,8);//
        emit Read_Holding_Registers(166,5);//
        emit Read_Holding_Registers(178,3);//
        emit Read_Holding_Registers(182,2);//
      });

    QTimer::singleShot(450, this, [this]{
        emit Read_Holding_Registers(185,1);//
        emit Read_Holding_Registers(188,3);//
        emit Read_Holding_Registers(194,6);//
        emit Read_Holding_Registers(202,1);//
        emit Read_Holding_Registers(204,1);//
        emit Read_Holding_Registers(206,2);//
        emit Read_Holding_Registers(208,7);//
        emit Read_Holding_Registers(140,1);//注意
      });

    QTimer::singleShot(600, this, [this]{
        // 读输入寄存器
        emit Read_Input_Registers(1,1);//
        emit Read_Input_Registers(7,2);//
        emit Read_Input_Registers(13,3);//
//        emit Read_Input_Registers(38,6);//
        emit Read_Input_Registers(56,8);//
        emit Read_Input_Registers(64,16);//
        emit Read_Input_Registers(82,8);//
        emit Read_Input_Registers(94,1);//
      });

      QTimer::singleShot(750, this, [this]{
          emit Read_Input_Registers(80,4);//
          emit Read_Input_Registers(90,4);//
          // 读线圈
          emit Read_Bits_Registers(0,16);//
          emit Read_Bits_Registers(40,24);//
          emit Read_Bits_Registers(63,1);//
          // 读离散输入
          emit Read_DiscreteInputs_Registers(0,2);//
          Receive_timer->start(2500);
      });
}
/**
 * @brief 显示提示信息（弹窗1秒后自动关闭）
 * @param error 提示文本
 */
void MainWindow::InformPlayHandle(const QString &error)
{
    QMessageBox *msgBox = new QMessageBox(QMessageBox::Information, "提示", error, QMessageBox::Ok, this);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
    int x = (this->width() - msgBox->sizeHint().width()) / 2;
    int y = (this->height() - msgBox->sizeHint().height()) / 2;
    msgBox->move(x, y);
    msgBox->show();
    // 1秒后关闭
    QTimer::singleShot(500, msgBox, &QMessageBox::close);
}

/**
 * @brief 导航列表当前行改变时的槽函数，切换界面
 * @param currentRow 当前行索引（0~7，对应界面索引+1）
 */
void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    screenchang_handle(currentRow+1);
}

void MainWindow::modbusstate_chang(bool connected)
{
    modbus_static=connected;
    if(connected==false)
    {
        Receive_timer->stop();
    }
    else if(connected==true)
    {
        Receive_timer->start(2500);
        // 初始读取一些关键寄存器
//        InitDataPlay();
        InformPlayHandle("从机已连接！");
    }
}

