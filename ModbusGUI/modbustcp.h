#ifndef MODBUSTCP_H
#define MODBUSTCP_H
#include "modbus/modbus.h"
#include <QDebug>
#include <QObject>
#include <QVector>
#include <QString>
#include <QMessageBox>
#include <QMutex>

class MainWindow;

class ModbusWorker_Tcp : public QObject
{
    Q_OBJECT

public:
    explicit ModbusWorker_Tcp(QObject *parent = nullptr);
    ~ModbusWorker_Tcp();

signals:
    void screen_dataReceived(int screen,const QVector<quint16> &data,QString str);
    void errorOccurred(const QString &error);
    void connectionStatusChanged(bool connected);
    void writeSingleRegister_handle(const QString &error);

public slots:
    void readHoldingRegisters(int screen, int slaveAddress, int startAddr, int numRegs);
    void writeSingleRegister(int screen, int slaveAddress, int regAddr, quint16 value);
    void reconnect();
private:

    modbus_t *m_tcp=nullptr;
    bool mtcp_running=false;

    void initModbus_Tcp(const QString &ip, int port);
    void stop();
    QMutex mtcp_modbusMutex; // 递归锁
    bool mtcp_reconnecting = false;

    QString mtcp_ip = "192.168.1.100"; // 默认IP
    int mtcp_port = 502;                // 默认端口 

};

#endif // MODBUSTCP_H
