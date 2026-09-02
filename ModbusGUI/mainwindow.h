#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modbusrtu.h"
#include <QThread>
#include <QStackedWidget>
#include <QTimer>
#include "zhuye.h"
#include "caxun.h"
#include "screen_3.h"
#include "yunxing.h"
#include "logindlg.h"
#include "sezhi.h"
#include "tiaoshi.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QTimer"
#include "sqlitemanager.h"
#include "dengru.h"
#include "QStringList"
#include <QMessageBox>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();  
    SqliteManager dbManager;
    Ui::MainWindow *ui;

    ModbusWorker *m_worker;
    int YH_QuanXian;
    int num_lcd_time=0;
    bool ShiZhongSeZhi=0;

//    void InitDataPlay();
    void SetPushButton(bool state,QPushButton *pushbotton);
    void SetQCheckBox(bool state,QCheckBox* cheackbox);
    void SetPushButtonText(int state,QPushButton *pushbotton,QStringList ZhuangTai);

public slots:
    void Modbus_handleData_16bit(const QVector<uint16_t> &data,QString str);
    void Modbus_handleData_8bit(const QVector<uint8_t> &data,QString str);
   // void Modbus_writehandle(const QString &error);
    void screenchang_handle(int gotoscreen);
    void X1s_Timer_Handle();
    void Receive_timer_Handle();
    void on_listWidget_currentRowChanged(int currentRow);
    void InformPlayHandle(const QString &error);
    void modbusstate_chang(bool connected);

signals:
    void WriteHoldRegisters(int regAddr, uint16_t value);
    void Trans_slaveaddress(int addr);
    void Read_Holding_Registers(int startAddr, int numRegs);
    void Read_Input_Registers(int startAddr, int numRegs);
    void Read_DiscreteInputs_Registers(int startAddr, int numRegs);
    void Read_Bits_Registers(int startAddr, int numRegs);
    void modbus_reconnet();

private:
    QThread *m_workerThread;
    QTimer *X1s_timer;
    QTimer *Receive_timer;
    Zhu_Ye_Screen *ZhuYe_screen;
    Ca_Xun_Screen *CaXun_screen;
    screen_3 *GuanLi_screen;
    Se_Zhi_Screen *SeZhi_screen;
    Yun_Xing_Screen *Yunxing_screen;
    loginDlg *dlg_screen;
    Tiao_Shi_Screen *Tiaoshi_screen;
    Deng_Lv_Screen *Dengru_screen;
    QStringList DangQianZhuangTai;
    QStringList DangQianLiangCheng;
    QStringList DangQianLiangCheng_len;
    QStringList ShuJuLeiXing;
    QStringList YunXingBuZhou;
    QStringList JianCheMoShi;
    QStringList BaoJingZhuangTai;
    QStringList DuoBuTiaoShi;
    QStringList ZhuangTai;
    QStringList YuJingZhuangTai;
    //标定数据表格数据
    float Biaodingnongdu=0;
    float Xiguangdu=0;
    float Yichixiang=0;
    float Changshuxiang=0;
    QString Biaodingleixing;
    int Rizhibaojing=100;
    int Caozuorizhi=100;
    bool Hold140=false;

    int slaveAddress=1;//Modubs-rtu地址
    bool isUserInteractionEvent(QEvent *event);
    float Uint16CoverFloat(uint16_t Low,uint16_t Hig);
    static const QString Greed_BUTTON_STYLE;
    static const QString Red_BUTTON_STYLE;
    bool modbus_static=false;

};
#endif // MAINWINDOW_H
