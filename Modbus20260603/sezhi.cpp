#include "sezhi.h"
#include "ui_sezhi.h"
#include "mainwindow.h"
#include <QDateTime>
#include <QLineEdit>
#include <QProcess>

/**
 * @brief 设置界面构造函数
 * @param parent 父窗口（通常为 MainWindow）
 *
 * 初始化 UI 组件，创建数字键盘，配置表格控件并初始化 QLineEdit 数组。
 */
Se_Zhi_Screen::Se_Zhi_Screen(QWidget *parent) :
    QWidget(parent), ui(new Ui::SeZhi), my_mainwidget(static_cast<MainWindow*>(parent))
{
    ui->setupUi(this);

    // 创建数字键盘，并连接到键盘确认信号的槽函数
    keyboard_sezhi = new DigitalKeyboardcpp(ui->widget1);
    connect(keyboard_sezhi, &DigitalKeyboardcpp::OkKeyPressed, this, &Se_Zhi_Screen::Keypressed_handl_SeZhi);
    ui->stackedWidget->setCurrentIndex(0);  // 默认显示第一个设置页面

    // 设置表格的表头不可点击
    ui->tableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->tableWidget->verticalHeader()->setSectionsClickable(false);

    // 动态创建表格中的 QLineEdit 控件，并保存到二维向量 SinJiTiJi_line 中
    int rowCount = ui->tableWidget->rowCount();
    int colCount = ui->tableWidget->columnCount();
    SinJiTiJi_line.resize(rowCount);
    for (int row = 0; row < rowCount; ++row) {
        SinJiTiJi_line[row].resize(colCount);
        for (int col = 0; col < colCount; ++col) {
            QLineEdit *line = new QLineEdit(this);
            line->setAlignment(Qt::AlignCenter);
            ui->tableWidget->setCellWidget(row, col, line);
            SinJiTiJi_line[row][col] = line;
        }
    }
}

/**
 * @brief 析构函数，释放 UI 资源
 */
Se_Zhi_Screen::~Se_Zhi_Screen()
{
    delete ui;
}

/**
 * @brief 设置系统时间（Linux 平台）
 * @param dt 要设置的日期时间
 * @return 设置成功返回 true，失败返回 false
 *
 * 通过调用 date 命令设置系统时间，并可选地将时间同步到硬件时钟（hwclock -w）。
 * 注意：此函数仅在 Linux 下有效，且需要 root 权限。
 */
bool Se_Zhi_Screen::setSystemTimeWithDate(const QDateTime &dt) {
    // 构造 date 命令：date -s "YYYY-MM-DD HH:MM:SS"
    QString cmd = QString("date -s \"%1\"").arg(dt.toString("yyyy-MM-dd HH:mm:ss"));

    QProcess process;
    process.start("sh", QStringList() << "-c" << cmd);
    if (!process.waitForFinished(2000)) { // 等待最多2秒
        qDebug() << "set time command timed out";
        return false;
    }

    if (process.exitCode() != 0) {
        qDebug() << "Failed to set time:" << process.errorString();
        qDebug() << "Error output:" << process.readAllStandardError();
        return false;
    }

    // 将系统时间写入硬件时钟（RTC）
    QProcess hwclock;
    hwclock.start("hwclock", QStringList() << "-w" << "--localtime");
    hwclock.waitForFinished();

    return true;
}

/**
 * @brief 处理数字键盘确认键（OkKeyPressed）的槽函数
 * @param text         键盘输入的文本
 * @param focusedWidget 输入时获得焦点的控件（通常为 QLineEdit）
 *
 * 根据当前焦点控件类型，验证输入数据，并将值写入相应的 Modbus 寄存器或数据库。
 * 支持整数、浮点数以及密码等不同类型的数据处理。
 */
void Se_Zhi_Screen::Keypressed_handl_SeZhi(QString text, QWidget* focusedWidget)
{
    if (!focusedWidget) return;
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(focusedWidget);

    // 处理密码输入框（ui->lineEdit）
    if (lineEdit == ui->lineEdit)
    {
        if (!text.isEmpty())
        {
            // 将密码保存到数据库
            QMap<QString, QVariant> setData;
            setData["str1"] = "密码";
            setData["str2"] = text;
            my_mainwidget->dbManager.deleteRecord("[set]", "str1", "密码");
            my_mainwidget->dbManager.insertRecord("[set]", setData);
            my_mainwidget->InformPlayHandle("密码设置成功!");
        }
        else
        {
            my_mainwidget->InformPlayHandle("密码设置错误!");
            ui->lineEdit->setFocus();
        }
    }
    // 处理整数型输入框（包括部分表格单元格）
    else if ((lineEdit == ui->LineEdit_16) || (lineEdit == ui->LineEdit_15) ||
             (lineEdit == ui->LineEdit_14) || (lineEdit == ui->LineEdit_13) ||
             (lineEdit == ui->LineEdit_12) || (lineEdit == ui->LineEdit_3) ||
             (lineEdit == ui->LineEdit_4) || (lineEdit == ui->LineEdit_7) ||
             (lineEdit == ui->LineEdit_8) || (lineEdit == ui->lineEdit_14) ||
             (lineEdit == qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(0, 0))) ||
             (lineEdit == SinJiTiJi_line[0][0]) || (lineEdit == SinJiTiJi_line[0][1]) ||
             (lineEdit == SinJiTiJi_line[0][2]) || (lineEdit == SinJiTiJi_line[0][3]) ||
             (lineEdit == SinJiTiJi_line[1][1]) || (lineEdit == SinJiTiJi_line[1][0]) ||
             (lineEdit == SinJiTiJi_line[1][2]) || (lineEdit == SinJiTiJi_line[1][3])||
             (lineEdit == ui->LineEdit_10) || (lineEdit == ui->lineEdit_11))
    {
        bool ok;
        uint16_t value = text.toUShort(&ok);
        // 验证输入：非空、仅数字、值 ≥1
        if (!text.isEmpty() && text.contains(QRegularExpression("^\\d+$")) && value >= 1 && ok)
        {
            lineEdit->setText(text);
            // 根据不同的控件，发送不同的 Modbus 写寄存器信号
            if (lineEdit == ui->LineEdit_3)        { emit WriteHoldRegisters_SeZhi(7, value); }
            else if (lineEdit == ui->LineEdit_4)   { emit WriteHoldRegisters_SeZhi(119, value); }
            else if (lineEdit == ui->LineEdit_7)   { emit WriteHoldRegisters_SeZhi(121, value); }
            else if (lineEdit == ui->LineEdit_8)   { emit WriteHoldRegisters_SeZhi(206, value); }
            else if (lineEdit == ui->LineEdit_12)  { emit WriteHoldRegisters_SeZhi(44, value); }
            else if (lineEdit == ui->LineEdit_13)  { emit WriteHoldRegisters_SeZhi(43, value); }
            else if (lineEdit == ui->LineEdit_14)  { emit WriteHoldRegisters_SeZhi(45, value); }
            else if (lineEdit == ui->LineEdit_15)  { emit WriteHoldRegisters_SeZhi(40, value); emit ReadHoldRegisters_SeZhi(40, 1); }
            else if (lineEdit == ui->LineEdit_16)  { emit WriteHoldRegisters_SeZhi(62, value); }
            else if (lineEdit == ui->lineEdit_14)  { emit WriteHoldRegisters_SeZhi(162, value); }
            else if (lineEdit == ui->LineEdit_10)  { emit WriteHoldRegisters_SeZhi(110, value); }
            else if (lineEdit == ui->lineEdit_11)  { emit WriteHoldRegisters_SeZhi(109, value); }
            else if (lineEdit == SinJiTiJi_line[0][0]) { emit WriteHoldRegisters_SeZhi(156, value);emit ReadHoldRegisters_SeZhi(156, 1); }
            else if (lineEdit == SinJiTiJi_line[0][1]) { emit WriteHoldRegisters_SeZhi(157, value);emit ReadHoldRegisters_SeZhi(157, 1); }
            else if (lineEdit == SinJiTiJi_line[0][2]) { emit WriteHoldRegisters_SeZhi(158, value);emit ReadHoldRegisters_SeZhi(158, 1);}
            else if (lineEdit == SinJiTiJi_line[0][3]) { emit WriteHoldRegisters_SeZhi(159, value);emit ReadHoldRegisters_SeZhi(159, 1); }
            else if (lineEdit == SinJiTiJi_line[1][0]) { emit WriteHoldRegisters_SeZhi(146, value);emit ReadHoldRegisters_SeZhi(146, 1); }
            else if (lineEdit == SinJiTiJi_line[1][1]) { emit WriteHoldRegisters_SeZhi(147, value);emit ReadHoldRegisters_SeZhi(147, 1); }
            else if (lineEdit == SinJiTiJi_line[1][2]) { emit WriteHoldRegisters_SeZhi(148, value);emit ReadHoldRegisters_SeZhi(148, 1); }
            else if (lineEdit == SinJiTiJi_line[1][3]) { emit WriteHoldRegisters_SeZhi(155, value);emit ReadHoldRegisters_SeZhi(155, 1); }
        }
        else
        {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需整数)!");
            lineEdit->setFocus();
        }
    }
    // 处理其他整数输入框（如时钟设置界面的临时输入）
    else if ((lineEdit == ui->lineEdit_3) || (lineEdit == ui->lineEdit_4) ||
             (lineEdit == ui->lineEdit_5) || (lineEdit == ui->lineEdit_11) ||
             (lineEdit == ui->lineEdit_12))
    {
        if (!text.isEmpty() && text.contains(QRegularExpression("^\\d+$")))
        {
            lineEdit->setText(text);
        }
        else
        {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需整数)!!");
            lineEdit->setFocus();
        }
    }
    // 处理浮点数输入框
    else if ((lineEdit == ui->LineEdit_17) || (lineEdit == ui->lineEdit_13) ||
             (lineEdit == ui->lineEdit_15))
    {
        bool ok;
        float value = text.toFloat(&ok);
        if (!text.isEmpty() && ok)
        {
            lineEdit->setText(text);
            if (lineEdit == ui->LineEdit_17)      { emit WriteHoldFloatRegisters_SeZhi(124, value); }
            if (lineEdit == ui->lineEdit_13)      { emit WriteHoldFloatRegisters_SeZhi(160, value); }
            if (lineEdit == ui->lineEdit_15)      { emit WriteHoldFloatRegisters_SeZhi(104, value); emit ReadHoldRegisters_SeZhi(104, 2); }
        }
        else
        {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需浮点数)!");
            lineEdit->setFocus();
        }
    }
}

/**
 * @brief 重置密码按钮槽函数
 *
 * 将数据库中的密码重置为默认值 "123456"。
 */
void Se_Zhi_Screen::on_pushButton_clicked()
{
    QMap<QString, QVariant> setData;
    my_mainwidget->dbManager.deleteRecord("[set]", "str1", "密码");
    setData["str1"] = "密码";
    setData["str2"] = "123456";
    my_mainwidget->dbManager.insertRecord("[set]", setData);
    my_mainwidget->InformPlayHandle("密码重置成功!");
}

/**
 * @brief 设置页面列表切换槽函数
 * @param currentRow 当前选中的行索引
 *
 * 根据选中的设置项，切换 stackedWidget 的当前页面，
 * 并将数字键盘移动到当前页面中对应的 widget 上，以方便输入。
 */
void Se_Zhi_Screen::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow + 1);
    switch (currentRow + 1) {
    case 1:
        keyboard_sezhi->setParent(ui->widget_2);
        break;
    case 2:
    {
        keyboard_sezhi->setParent(ui->widget_10);
        // 获取当前系统时间，用于设置 placeholder
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QDate datePart = currentDateTime.date();
        int year = datePart.year();
        int month = datePart.month();
        int day = datePart.day();
        QTime timePart = currentDateTime.time();
        int hour = timePart.hour();
        int minute = timePart.minute();

        ui->lineEdit_3->setPlaceholderText(QString::number(year));
        ui->lineEdit_4->setPlaceholderText(QString::number(month));
        ui->lineEdit_5->setPlaceholderText(QString::number(day));
        ui->lineEdit_11->setPlaceholderText(QString::number(hour));
        ui->lineEdit_12->setPlaceholderText(QString::number(minute));
        break;
    }
    case 3:
        keyboard_sezhi->setParent(ui->widget_11);
        break;
    case 4:
        keyboard_sezhi->setParent(ui->widget_3);
        break;
    case 5:
        keyboard_sezhi->setParent(ui->widget_6);
        break;
    case 6:
        keyboard_sezhi->setParent(ui->widget);
        break;
    default:
        break;
    }
}

/**
 * @brief 系统时间设置确认按钮槽函数
 *
 * 从输入框中读取年月日时分，组合成 QDateTime 并调用 setSystemTimeWithDate 进行设置。
 * 设置成功后，更新 placeholder 显示当前系统时间，并给出提示。
 */
void Se_Zhi_Screen::on_pushButton_3_clicked()
{
    bool ok1, ok2, ok3, ok4, ok5;
    uint16_t N, Y, R, S, F;
    // 检查输入框是否都非空
    if (!ui->lineEdit_3->text().isEmpty() && !ui->lineEdit_4->text().isEmpty() &&
        !ui->lineEdit_5->text().isEmpty() && !ui->lineEdit_11->text().isEmpty() &&
        !ui->lineEdit_12->text().isEmpty()) {
        N = ui->lineEdit_3->text().toUShort(&ok1);
        Y = ui->lineEdit_4->text().toUShort(&ok2);
        R = ui->lineEdit_5->text().toUShort(&ok3);
        S = ui->lineEdit_11->text().toUShort(&ok4);
        F = ui->lineEdit_12->text().toUShort(&ok5);
        // 清空输入框
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_11->clear();
        ui->lineEdit_12->clear();

        if (ok1 && ok2 && ok3 && ok4 && ok5) {
            QDateTime newTime(QDate(N, Y, R), QTime(S, F, 0));
            if (setSystemTimeWithDate(newTime)) {
                // 设置成功后，获取当前系统时间并更新 placeholder
                QDateTime currentDateTime = QDateTime::currentDateTime();
                QDate datePart = currentDateTime.date();
                int year = datePart.year();
                int month = datePart.month();
                int day = datePart.day();
                QTime timePart = currentDateTime.time();
                int hour = timePart.hour();
                int minute = timePart.minute();
                ui->lineEdit_3->setPlaceholderText(QString::number(year));
                ui->lineEdit_4->setPlaceholderText(QString::number(month));
                ui->lineEdit_5->setPlaceholderText(QString::number(day));
                ui->lineEdit_11->setPlaceholderText(QString::number(hour));
                ui->lineEdit_12->setPlaceholderText(QString::number(minute));
                QVector<uint16_t> data = {
                    static_cast<uint16_t>(year),
                    static_cast<uint16_t>(month),
                    static_cast<uint16_t>(day),
                    static_cast<uint16_t>(hour),
                    static_cast<uint16_t>(minute),
                    0
                };
                emit WriteMultipleHoldRegisters_SeZhi(12, data);
                emit WriteHoldRegisters_SeZhi(11, 1);
                QTimer::singleShot(3000, this, [this]() {
                    // 3秒后执行的代码
                    emit ReadHoldRegisters_SeZhi(12,6);
                });
            } else {
                my_mainwidget->InformPlayHandle("时间设置失败!");
            }
        }
    } else {
        my_mainwidget->InformPlayHandle("时钟数据有空缺!");
    }
}
