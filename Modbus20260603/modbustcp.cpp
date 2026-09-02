#include "modbustcp.h"
#include "modbus/modbus.h"
#include <QDebug>
#include <QVector>
#include <QString>
#include <QTimer>
#include "mainwindow.h"


ModbusWorker_Tcp::ModbusWorker_Tcp(QObject *parent) : QObject(parent)
{
    initModbus_Tcp(mtcp_ip, mtcp_port); // 使用TCP参数初始化
    qRegisterMetaType<QVector<quint16>>("QVector<quint16>");
}

ModbusWorker_Tcp::~ModbusWorker_Tcp()
{
    stop();
}

void ModbusWorker_Tcp::initModbus_Tcp(const QString &ip, int port)
{
     QMutexLocker locker(&mtcp_modbusMutex);
     mtcp_ip=ip;
     mtcp_port=port;

     if(m_tcp)
     {
         modbus_close(m_tcp);
         modbus_free(m_tcp);
         m_tcp=nullptr;
     }
     m_tcp=modbus_new_tcp(ip.toUtf8().constData(),port);
     if (!m_tcp) {
        emit errorOccurred("创建TCP上下文失败: " + QString(modbus_strerror(errno)));
        return;
     }
     modbus_set_response_timeout(m_tcp, 3, 0);
     if (modbus_connect(m_tcp))
     {
         modbus_free(m_tcp);
         m_tcp = nullptr;
         emit errorOccurred("TCP连接失败: " + QString(modbus_strerror(errno)));
         return;
     }
     mtcp_running = true;
     emit connectionStatusChanged(true);
}

void ModbusWorker_Tcp::readHoldingRegisters(int screen, int slaveAddress, int startAddr, int numRegs)
{
    if (!mtcp_running || mtcp_reconnecting) {
        emit errorOccurred("未初始化或已停止");
        return;
    }
    QMutexLocker locker(&mtcp_modbusMutex);
    if (!m_tcp) {
        emit errorOccurred("Modbus上下文未初始化");
        return;
    }

    modbus_set_slave(m_tcp, slaveAddress);
    QVector<quint16> buffer(numRegs);

    int rc = modbus_read_registers(m_tcp, startAddr, numRegs, buffer.data());
    if (rc == numRegs) {
        QString read_str = QString("%1%2%3%4")
            .arg(slaveAddress, 2, 16, QLatin1Char('0'))
            .arg("03")
            .arg(startAddr, 4, 16, QLatin1Char('0'))
            .arg(numRegs, 4, 16, QLatin1Char('0'));
        emit screen_dataReceived(screen, buffer, read_str);
    } else {
        emit errorOccurred("数据读取失败: " + QString(modbus_strerror(errno)));
        QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
    }
}

void ModbusWorker_Tcp::writeSingleRegister(int screen, int slaveAddress, int regAddr, quint16 value)
{
    Q_UNUSED(screen)
       if (!mtcp_running || mtcp_reconnecting)
       {
           emit errorOccurred("未初始化或已停止");
           return;
       }

       QMutexLocker locker(&mtcp_modbusMutex);
       if (!m_tcp) {
           emit errorOccurred("Modbus上下文未初始化");
           return;
       }

       modbus_set_slave(m_tcp, slaveAddress);
       int rc = modbus_write_register(m_tcp, regAddr, value);

       if (rc == 1) {
           emit writeSingleRegister_handle("数据已发送");
       } else {
           emit errorOccurred("数据写入失败: " + QString(modbus_strerror(errno)));
           QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
       }
}

void ModbusWorker_Tcp::reconnect()
{
    if (mtcp_reconnecting) return;

        mtcp_reconnecting = true;
        emit connectionStatusChanged(false);

        {
            QMutexLocker locker(&mtcp_modbusMutex);
            if (m_tcp) {
                modbus_close(m_tcp);
                modbus_free(m_tcp);
                m_tcp = nullptr;
            }
        }

        QTimer::singleShot(1000, this, [this]() {
            initModbus_Tcp(mtcp_ip, mtcp_port); // 使用保存的参数重连
            mtcp_reconnecting = false;
        });
}



void ModbusWorker_Tcp::stop()
{
    QMutexLocker locker(&mtcp_modbusMutex);
    if (m_tcp) {
        modbus_close(m_tcp);
        modbus_free(m_tcp);
        m_tcp = nullptr;
    }
    mtcp_running = false;
    mtcp_reconnecting = false;
    emit connectionStatusChanged(false);
}
