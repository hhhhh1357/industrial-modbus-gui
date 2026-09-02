#include "modbusrtu.h"
#include "modbus/modbus.h"
#include <QDebug>
#include <QVector>
#include <QString>
#include <QByteArray>
#include <unistd.h>
#include "mainwindow.h"
#include <QMutex>
#include <QtEndian>  // 用于字节序转换
/**
 * @brief ModbusWorker 构造函数
 * @param parent 父对象指针
 *
 * 根据 RTU_TCP 标志决定使用 RTU 串口还是 TCP 连接：
 * - RTU_TCP == false：调用 initModbus 初始化串口（根据平台选择设备文件）
 * - RTU_TCP == true：调用 initModbus_Tcp 初始化 TCP 连接
 *
 * 默认 RTU_TCP = false，即使用串口。
 */
ModbusWorker::ModbusWorker(int slaveAddress, QObject *parent)
    : QObject(parent), slave_addr(slaveAddress)
{
}
/**
 * @brief 析构函数，停止 Modbus 并释放资源
 */
ModbusWorker::~ModbusWorker()
{
    stop();
}
/**
 * @brief 初始化 Modbus RTU 串口连接
 * @param port     串口设备名，如 "/dev/ttyS0"
 * @param baud     波特率
 * @param parity   校验位：'N' 无校验，'E' 偶校验，'O' 奇校验
 * @param dataBits 数据位（通常为 8）
 * @param stopBits 停止位（通常为 1）
 *
 * 创建 libmodbus RTU 上下文，设置超时、串口模式，并尝试连接。
 * 成功后设置 m_running = true 并发出 connectionStatusChanged(true)。
 * 失败时发出 errorOccurred 信号。
 */
void ModbusWorker::initModbus(const QString &port, int baud, char parity, int dataBits, int stopBits)
{
    QMutexLocker locker(&m_modbusMutex);
    if (m_ctx) {
        modbus_close(m_ctx);
        modbus_free(m_ctx);
        m_ctx = nullptr;
    }

    m_ctx = modbus_new_rtu(port.toUtf8().constData(), baud, parity, dataBits, stopBits);
    if (!m_ctx) {
        emit errorOccurred("创建RTU上下文失败: " + QString(modbus_strerror(errno)));
        return;
    }
    // 1. 设置从站地址（必须在 connect 前）
    if (modbus_set_slave(m_ctx, slave_addr) == -1) {
        emit errorOccurred("设置从站地址失败: " + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
//    // 2. 设置 RS‑232 模式（必须在 connect 前）
    if (modbus_rtu_set_serial_mode(m_ctx, MODBUS_RTU_RS232) == -1) {
        emit errorOccurred("设置485模式失败:" + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
    // 3. 设置 RTS 控制（必须在 connect 前）
    if (modbus_rtu_set_rts(m_ctx, MODBUS_RTU_RTS_NONE) == -1) {
        emit errorOccurred("设置RTS失败: " + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
    // 4. 设置超时（建议在 connect 前，也可 connect 后，但提前设置更稳妥）
    if (modbus_set_byte_timeout(m_ctx, 0, 100000) == -1) {
        emit errorOccurred("设置字节超时失败: " + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
    if (modbus_set_response_timeout(m_ctx, 0, 200000) == -1) {
        emit errorOccurred("设置响应超时失败: " + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
    if (modbus_set_error_recovery(m_ctx, MODBUS_ERROR_RECOVERY_LINK) == -1) {
        emit errorOccurred("设置错误恢复失败: " + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
    // 5. 最后连接
    if (modbus_connect(m_ctx) == -1) {
        emit errorOccurred("连接失败: " + QString(modbus_strerror(errno)));
        modbus_free(m_ctx);
        m_ctx = nullptr;
        return;
    }
//    modbus_set_debug(m_ctx, 1);
    // 连接成功后的验证测试（可选）
    uint16_t testVal;
    if (modbus_read_registers(m_ctx, 0, 1, &testVal) == 1) {
        emit connectionStatusChanged(true);
        m_running = true;
        return;
    }
    emit errorOccurred("从机无响应");
    modbus_close(m_ctx);
    modbus_free(m_ctx);
    m_ctx = nullptr;
}

/**
 * @brief 读取保持寄存器（功能码 0x03）
 * @param startAddr 起始地址
 * @param numRegs   寄存器数量
 *
 * 发送读取请求，成功时将接收到的数据（16位向量）和请求报文通过 DataReceived_16bit 信号发出。
 * 失败时发出 errorOccurred 信号并触发重连。
 */
void ModbusWorker::ReadHoldingRegisters(int startAddr, int numRegs)
{
    if (!m_running || m_reconnecting)
    {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx)
    {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx,slave_addr);// 设置从站地址
    modbus_flush(m_ctx);// 清空缓冲区，避免残留数据干扰
    QVector<uint16_t> buffer(numRegs);
    int rc = modbus_read_registers(m_ctx, startAddr, numRegs, buffer.data());
    // 构造请求字符串，用于信号传递（格式：从站地址 + 功能码 + 起始地址 + 寄存器数量）
    QString read_str = QString("%1%2%3%4")
        .arg(slave_addr, 2, 16, QLatin1Char('0'))  // 从站地址，2位十六进制，补零
        .arg("03")                                   // 功能码03（固定）
        .arg(startAddr, 4, 16, QLatin1Char('0'))     // 起始地址，4位十六进制，补零
        .arg(numRegs, 4, 16, QLatin1Char('0'));      // 寄存器数量，4位十六进制，补零

    if (rc == numRegs)
    {
//        qDebug() << "ReadHoldingRegisters success, request:" << read_str;
//        qDebug() << "Received values:" << buffer;
        emit DataReceived_16bit(buffer,read_str);
    }
//    else
//    {
//        // 输出“发出的请求” + “接收到的错误描述”
//        QString errorMsg = QString("读取失败: %1\n发送请求: [%2]")
//                       .arg(QString(modbus_strerror(errno)))
//                       .arg(read_str);
//         qDebug() << errorMsg;
//       emit errorOccurred("保持寄存器读取失败: " + QString(modbus_strerror(errno)));
////       QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//    }
}
/**
 * @brief 读取线圈（功能码 0x01）
 * @param startAddr 起始地址
 * @param numRegs   线圈数量
 *
 * 发送读取请求，成功时将接收到的数据（8位向量）和请求报文通过 DataReceived_8bit 信号发出。
 * 失败时发出 errorOccurred 信号并触发重连。
 */
void ModbusWorker::ReadBitsRegisters(int startAddr, int numRegs)
{
    if (!m_running || m_reconnecting)
    {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx)
    {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx,slave_addr);
    modbus_flush(m_ctx);//新加
    QVector<uint8_t> buffer(numRegs);
    int rc = modbus_read_bits(m_ctx, startAddr, numRegs, buffer.data());//下位机地址变化，把线圈寄存器地址加1为正确地址
    QString read_str = QString("%1%2%3%4")
        .arg(slave_addr, 2, 16, QLatin1Char('0'))  // 从站地址，8位十六进制，补零
        .arg("01")                                   // 功能码01（固定）
        .arg(startAddr, 4, 16, QLatin1Char('0'))     // 起始地址，16位十六进制，补零-----startAddr不加1，不用改变mainwindow.c中的线圈寄存器处理逻辑
        .arg(numRegs, 4, 16, QLatin1Char('0'));      // 寄存器数量，16位十六进制，补零
    if (rc == numRegs)
    {
        emit DataReceived_8bit(buffer,read_str);
    }
//    else
//    {
//        // 输出“发出的请求” + “接收到的错误描述”
//        QString errorMsg = QString("读取失败: %1\n发送请求: [%2]")
//                       .arg(QString(modbus_strerror(errno)))
//                       .arg(read_str);
//         qDebug() << errorMsg;
//       emit errorOccurred("线圈数据读取失败: " + QString(modbus_strerror(errno)));
////       QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//    }
}
/**
 * @brief 读取离散输入（功能码 0x02）
 * @param startAddr  起始地址
 * @param numInputs  输入数量
 *
 * 发送读取请求，成功时将接收到的数据（8位向量）和请求报文通过 DataReceived_8bit 信号发出。
 * 失败时发出 errorOccurred 信号并触发重连。
 */
void ModbusWorker::ReadDiscreteInputs(int startAddr, int numInputs)
{
    if (!m_running || m_reconnecting) {emit errorOccurred("串口未连接!");return;}
    if (!m_ctx) {emit errorOccurred("Modbus未初始化！");return;}
    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx, slave_addr);
    modbus_flush(m_ctx);
    QVector<uint8_t> buffer(numInputs);
    int rc = modbus_read_input_bits(m_ctx, startAddr, numInputs, buffer.data());
    // 构造请求字符串（功能码为 02）
    QString read_str = QString("%1%2%3%4")
        .arg(slave_addr, 2, 16, QLatin1Char('0'))
        .arg("02")                                   // 功能码 02（读离散输入）
        .arg(startAddr, 4, 16, QLatin1Char('0'))
        .arg(numInputs, 4, 16, QLatin1Char('0'));
    if (rc == numInputs) {
        emit DataReceived_8bit(buffer, read_str);
    } /*else {
        // 输出“发出的请求” + “接收到的错误描述”
        QString errorMsg = QString("读取失败: %1\n发送请求: [%2]")
                       .arg(QString(modbus_strerror(errno)))
                       .arg(read_str);
         qDebug() << errorMsg;
        emit errorOccurred("离散输入读取失败: " + QString(modbus_strerror(errno)));
//        QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
    }*/
}
/**
 * @brief 读取输入寄存器（功能码 0x04）
 * @param startAddr 起始地址
 * @param numRegs   寄存器数量
 *
 * 发送读取请求，成功时将接收到的数据（16位向量）和请求报文通过 DataReceived_16bit 信号发出。
 * 失败时发出 errorOccurred 信号并触发重连。
 */
void ModbusWorker::ReadInputRegisters(int startAddr, int numRegs)
{
    if (!m_running || m_reconnecting)
    {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx)
    {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx, slave_addr);
    modbus_flush(m_ctx);   // 清空缓冲区，避免残留数据干扰

    QVector<uint16_t> buffer(numRegs);
    int rc = modbus_read_input_registers(m_ctx, startAddr, numRegs, buffer.data());
    // 构造请求字符串（功能码为 04）
    QString read_str = QString("%1%2%3%4")
        .arg(slave_addr, 2, 16, QLatin1Char('0'))
        .arg("04")                                   // 功能码 04（读输入寄存器）
        .arg(startAddr, 4, 16, QLatin1Char('0'))
        .arg(numRegs, 4, 16, QLatin1Char('0'));
    if (rc == numRegs)
    {

        emit DataReceived_16bit(buffer, read_str);
    }
//    else
//    {
//        // 输出“发出的请求” + “接收到的错误描述”
//        QString errorMsg = QString("读取失败: %1\n发送请求: [%2]")
//                       .arg(QString(modbus_strerror(errno)))
//                       .arg(read_str);
//         qDebug() << errorMsg;
//        emit errorOccurred("输入寄存器读取失败: " + QString(modbus_strerror(errno)));
////        QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//    }
}
/**
 * @brief 重连机制（在读取或写入失败时调用）
 *
 * 首先设置 m_reconnecting 标志，发出连接状态变化信号，
 * 然后释放原上下文，延迟 1 秒后重新调用 initModbus 或 initModbus_Tcp 尝试连接。
 * 连接成功后重置 m_reconnecting 标志。
 */
void ModbusWorker::reconnect()
{
    if (m_reconnecting) return;          // 已经在重连流程中
    m_reconnecting = true;
    m_running = false;
    emit connectionStatusChanged(false);
    // 如果已有定时器，取消之前的
    if (m_reconnectTimer) m_reconnectTimer->stop();
    if (!m_reconnectTimer) {
        m_reconnectTimer = new QTimer(this);
        m_reconnectTimer->setSingleShot(true);
        connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
            // 真正执行重连
            #ifdef IMX6_PLATFORM
                initModbus("/dev/ttyAS4", 115200, 'N', 8, 1);
            #else
                initModbus("/dev/ttyV1", 115200, 'N', 8, 1);
            #endif
            m_reconnecting = false;      // 重连尝试结束，无论成败
        });
    }
    m_reconnectTimer->start(1000);       // 延迟 1 秒后再试
}
/**
 * @brief 接收从站地址（由外部设置）
 * @param addr 从站地址（1~247）
 */
void ModbusWorker::Received_slaveaddress(int addr)
{
    if(addr!=slave_addr)
    {
        slave_addr=addr;
        reconnect();
    }

}
/**
 * @brief 写单个线圈（功能码 0x05）
 * @param coilAddr 线圈地址
 * @param value    值（true=ON，false=OFF）
 *
 * 写入成功时发出 WriteSingleRegister_handle 信号，失败时发出 errorOccurred 并触发重连。
 */
void ModbusWorker::WriteSingleCoil(int coilAddr, bool value)
{
    if (!m_running || m_reconnecting) {
            emit errorOccurred("串口未连接!");
            return;
        }
        if (!m_ctx) {
            emit errorOccurred("Modbus未初始化！");
            return;
        }

        QMutexLocker locker(&m_modbusMutex);
        modbus_set_slave(m_ctx, slave_addr);
        modbus_flush(m_ctx);

        int rc = modbus_write_bit(m_ctx, coilAddr, value ? 1 : 0);
//        if (rc == 1) {
//            emit WriteRegister_Inform("数据写入成功");
//        } else {
//            emit errorOccurred("线圈写入失败: " + QString(modbus_strerror(errno)));
////            QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//        }
}
/**
 * @brief 写多个线圈（功能码 0x0F）
 * @param startAddr 起始地址
 * @param values    布尔值向量（每个元素对应一个线圈的状态）
 *
 * 写入成功时发出 WriteSingleRegister_handle 信号，失败时发出 errorOccurred 并触发重连。
 */
void ModbusWorker::WriteMultipleCoils(int startAddr, const QVector<bool> &values)
{
    if (!m_running || m_reconnecting) {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx) {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    if (values.isEmpty()) {
        emit errorOccurred("写入数据为空");
        return;
    }

    // 将 QVector<bool> 转换为 uint8_t 数组（每个元素 0 或 1）
    QVector<uint8_t> bits(values.size());
    for (int i = 0; i < values.size(); ++i) {
        bits[i] = values[i] ? 1 : 0;
    }

    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx, slave_addr);
    modbus_flush(m_ctx);

    int rc = modbus_write_bits(m_ctx, startAddr, values.size(), bits.constData());
//    if (rc == values.size()) {
//        emit WriteRegister_Inform("多线圈写入成功");
//    } else {
//        emit errorOccurred("多线圈写入失败: " + QString(modbus_strerror(errno)));
////        QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//    }
}

/**
 * @brief 写单个保持寄存器（功能码 0x06）
 * @param regAddr 寄存器地址
 * @param value   16位无符号整数值
 *
 * 写入成功时发出 WriteSingleRegister_handle 信号，失败时发出 errorOccurred 并触发重连。
 */
void ModbusWorker::WriteHoldRegister(int regAddr, uint16_t value)
{
    if (!m_running || m_reconnecting)
    {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx)
    {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx,slave_addr);
    modbus_flush(m_ctx);
    int rc = modbus_write_register(m_ctx, regAddr,value);
//    if(regAddr==140 || regAddr==10){}
//    else{if (rc == 1){emit WriteRegister_Inform("数据已发送");}
//        else{emit errorOccurred("数据写入失败: " + QString(modbus_strerror(errno)));
////           QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//        }   }
}
/**
 * @brief 写一个32位浮点数到两个连续的保持寄存器（功能码 0x10）
 * @param startAddr 起始地址（高16位在此地址，低16位在下一地址）
 * @param value     浮点数值
 *
 * 将浮点数按大端序拆分为两个16位寄存器，然后使用 modbus_write_registers 写入。
 * 写入成功时发出 WriteSingleRegister_handle 信号，失败时发出 errorOccurred 并触发重连。
 */
void ModbusWorker::WriteHoldFloatRegister(int startAddr, float value)
{
    if (!m_running || m_reconnecting) {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx) {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    uint32_t floatBits;// 将浮点数按内存布局转换为32位整数
    memcpy(&floatBits, &value, sizeof(float));
    uint16_t regHigh = static_cast<uint16_t>(floatBits >> 16); // 大端序拆分：高16位在 startAddr，低16位在 startAddr+1
    uint16_t regLow  = static_cast<uint16_t>(floatBits & 0xFFFF);

    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx, slave_addr);
    modbus_flush(m_ctx);

    uint16_t registers[2] = {regLow,regHigh};//小端序（Little-Endian）
    int rc = modbus_write_registers(m_ctx, startAddr, 2, registers);

//    if (rc == 2) {
//        emit WriteRegister_Inform("数据已发送！");
//    } else {
//        emit errorOccurred("数据写入失败: " + QString(modbus_strerror(errno)));
//        QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//    }
}

/**
 * @brief 写多个保持寄存器（功能码 0x10）
 * @param startAddr 起始地址
 * @param values    要写入的16位无符号整数向量
 *
 * 成功时发出 WriteSingleRegister_handle 信号，失败时发出 errorOccurred 并触发重连。
 */
void ModbusWorker::WriteMultipleHoldRegisters(int startAddr, const QVector<uint16_t> &values)
{
    if (!m_running || m_reconnecting) {
        emit errorOccurred("串口未连接!");
        return;
    }
    if (!m_ctx) {
        emit errorOccurred("Modbus未初始化！");
        return;
    }
    if (values.isEmpty()) {
        emit errorOccurred("写入数据为空");
        return;
    }

    QMutexLocker locker(&m_modbusMutex);
    modbus_set_slave(m_ctx, slave_addr);
    modbus_flush(m_ctx);

    int rc = modbus_write_registers(m_ctx, startAddr, values.size(), values.constData());
//    if (rc == values.size()) {
//        emit WriteRegister_Inform("数据已发送");
//    } else {
//        emit errorOccurred("多寄存器写入失败: " + QString(modbus_strerror(errno)));
////        QMetaObject::invokeMethod(this, "reconnect", Qt::QueuedConnection);
//    }
}

void ModbusWorker::init_modbus_slot()
{
    #ifdef IMX6_PLATFORM
        initModbus("/dev/ttyAS4", 115200, 'N', 8, 1);
    #elif UBUNTU_PLATFORM
        initModbus("/dev/ttyV1", 115200, 'N', 8, 1);
    #endif
}

/**
 * @brief 停止 Modbus 通信，关闭并释放资源
 *
 * 关闭连接，释放上下文，并将运行标志和重连标志置为 false。
 * 发出 connectionStatusChanged(false) 信号。
 */
void ModbusWorker::stop()
{
    if (m_reconnectTimer) m_reconnectTimer->stop();
    QMutexLocker locker(&m_modbusMutex); // 使用统一的递归互斥锁
    if(m_ctx)
    {
        modbus_close(m_ctx);
        modbus_free(m_ctx);
        m_ctx=nullptr;
    }
    m_running=false;
    m_reconnecting=false;
    emit connectionStatusChanged(false);
}

