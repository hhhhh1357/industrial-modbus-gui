#ifndef MODBUSRTU_H
#define MODBUSRTU_H
#include "modbus/modbus.h"
#include <QDebug>
#include <QObject>
#include <QVector>
#include <QString>
#include <QMessageBox>
#include <QMutex>

class ModbusWorker  : public QObject
{
    Q_OBJECT
public:
    static ModbusWorker& instance(int slaveAddress, QObject *parent = nullptr){
        static ModbusWorker worker(slaveAddress, parent);
        return worker;
    }
    virtual ~ModbusWorker();

signals:
    void DataReceived_16bit(const QVector<uint16_t> &data,QString str);
    void DataReceived_8bit(const QVector<uint8_t> &data,QString str);
    void errorOccurred(const QString &error);
    void connectionStatusChanged(bool connected);
    void WriteRegister_Inform(const QString &error);

public slots:
    void ReadHoldingRegisters(int startAddr, int numRegs);
    void ReadBitsRegisters(int startAddr, int numRegs);
    void ReadDiscreteInputs(int startAddr, int numInputs);
    void ReadInputRegisters(int startAddr, int numRegs);
    void WriteHoldRegister(int regAddr, uint16_t value);
    void WriteHoldFloatRegister(int startAddr, float value);
    void reconnect();
    void Received_slaveaddress(int addr);
    void WriteSingleCoil(int coilAddr, bool value);
    void WriteMultipleCoils(int startAddr, const QVector<bool> &values);
    void WriteMultipleHoldRegisters(int startAddr, const QVector<uint16_t> &values);
    void init_modbus_slot();

private:

    modbus_t *m_ctx=nullptr;
    bool m_running=false;

    void initModbus(const QString &port, int baud, char parity, int dataBits, int stopBits);
    void stop();   

    QMutex m_modbusMutex; // 递归锁
    bool m_reconnecting = false;

    int slave_addr=0;
    QTimer *m_reconnectTimer = nullptr;

    // 2. 禁用拷贝+赋值（必写）
    explicit ModbusWorker(int slaveAddress,QObject *parent = nullptr);
    ModbusWorker(const ModbusWorker&) = delete;
    ModbusWorker& operator=(const ModbusWorker&) = delete;
};


#endif // MODBUSRTU_H
