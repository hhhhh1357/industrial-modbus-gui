#include "tiaoshi.h"
#include "ui_tiaoshi.h"
#include "mainwindow.h"
#include <QFormLayout>
#include "QButtonGroup"
#include "QGridLayout"

/**
 * @brief 调试界面构造函数
 * @param parent 父窗口（通常为 MainWindow）
 *
 * 初始化 UI 组件，创建数字键盘，配置按钮组和布局，并设置部分控件的初始状态。
 */
Tiao_Shi_Screen::Tiao_Shi_Screen(QWidget *parent) :
    QWidget(parent), ui(new Ui::tiaoshi), my_mainwidget(static_cast<MainWindow*>(parent))
{
    ui->setupUi(this);
    ui->stackedWidget_2->setCurrentIndex(0);  // 默认显示第一个调试页面

    // 创建数字键盘，并连接到键盘确认信号的槽函数
    keyboard_tiaoshi = new DigitalKeyboardcpp(ui->widget);
    connect(keyboard_tiaoshi, &DigitalKeyboardcpp::OkKeyPressed, this, &Tiao_Shi_Screen::tiaoshi_Keypressed_handle);

    // 禁用部分单选按钮（其状态由 Modbus 数据控制）
    ui->radioButton->setEnabled(false);
    ui->radioButton_2->setEnabled(false);
    ui->radioButton_3->setEnabled(false);
    ui->radioButton_4->setEnabled(false);

    // 创建两个非互斥按钮组，用于管理单选按钮的状态
    QButtonGroup *group1 = new QButtonGroup(this);
    group1->setExclusive(false);
    group1->addButton(ui->radioButton);
    group1->addButton(ui->radioButton_2);

    QButtonGroup *group2 = new QButtonGroup(this);
    group2->setExclusive(false);
    group2->addButton(ui->radioButton_3);
    group2->addButton(ui->radioButton_4);

    // 设置网格布局的最小列宽和行高，优化显示效果
    ui->gridLayout->setColumnMinimumWidth(0, 100);
    ui->gridLayout->setColumnMinimumWidth(1, 100);
    ui->gridLayout->setRowMinimumHeight(0, 40);
    ui->gridLayout->setRowMinimumHeight(1, 40);
}

/**
 * @brief 析构函数，释放 UI 资源
 */
Tiao_Shi_Screen::~Tiao_Shi_Screen()
{
    delete ui;
}

/**
 * @brief 左侧一级列表切换槽函数
 * @param currentRow 当前选中的行索引
 *
 * 切换 stackedWidget_2 的当前页面（调试主页面）。
 */
void Tiao_Shi_Screen::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget_2->setCurrentIndex(currentRow + 1);
    ui->stackedWidget_3->setCurrentIndex(0);  // 重置二级页面索引
}

/**
 * @brief 右侧二级列表切换槽函数
 * @param currentRow 当前选中的行索引
 *
 * 切换 stackedWidget_3 的当前页面（调试子页面），并将数字键盘移动到对应的父容器中。
 */
void Tiao_Shi_Screen::on_listWidget_2_currentRowChanged(int currentRow)
{
    ui->stackedWidget_3->setCurrentIndex(currentRow + 1);
    switch (currentRow + 1)
    {
    case 1:
        keyboard_tiaoshi->setParent(ui->widget_10);
        break;
    case 2:
        keyboard_tiaoshi->setParent(ui->widget_9);
        break;
    case 3:
        keyboard_tiaoshi->setParent(ui->widget_11);
        break;
    case 4:
        keyboard_tiaoshi->setParent(ui->widget_12);
        break;
    default:
        break;
    }
}

/**
 * @brief 处理数字键盘确认键（OkKeyPressed）的槽函数
 * @param text         键盘输入的文本
 * @param focusedWidget 输入时获得焦点的控件（通常为 QLineEdit）
 *
 * 根据当前焦点控件，验证输入数据，并将值写入相应的 Modbus 寄存器。
 * 支持整数（uint16_t）和浮点数（float）两种类型。
 */
void Tiao_Shi_Screen::tiaoshi_Keypressed_handle(QString text, QWidget* focusedWidget)
{
    if (!focusedWidget) return;
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(focusedWidget);

    // 处理整数型输入框
    if ((lineEdit == ui->lineEdit_6) || (lineEdit == ui->lineEdit_2) ||
        (lineEdit == ui->lineEdit_4) || (lineEdit == ui->lineEdit_5) ||
        (lineEdit == ui->LineEdit) || (lineEdit == ui->LineEdit_2) ||
        (lineEdit == ui->LineEdit_6)) {
        bool ok;
        uint16_t value = text.toUShort(&ok);
        // 验证：非空、纯数字、值 ≥1、转换成功
        if (!text.isEmpty() && text.contains(QRegularExpression("^\\d+$")) && value >= 1 && ok) {
            lineEdit->setText(text);
            // 根据不同的控件，发送对应的 Modbus 写寄存器信号
            if (lineEdit == ui->lineEdit_6)  { emit WriteHoldRegisters_TiaoShi(57, value); }
            else if (lineEdit == ui->lineEdit_2)  { emit WriteHoldRegisters_TiaoShi(41, value); }
            else if (lineEdit == ui->lineEdit_4)  { emit WriteHoldRegisters_TiaoShi(42, value); }
            else if (lineEdit == ui->lineEdit_5)  { emit WriteHoldRegisters_TiaoShi(40, value); emit ReadHoldRegisters_TiaoShi(40, 1); }
            else if (lineEdit == ui->LineEdit)    { emit WriteHoldRegisters_TiaoShi(54, value); }
            else if (lineEdit == ui->LineEdit_2)  { emit WriteHoldRegisters_TiaoShi(55, value); }
            else if (lineEdit == ui->LineEdit_6)  { emit WriteHoldRegisters_TiaoShi(19, value); }
        } else {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需整数)!");
            lineEdit->setFocus();
        }
    }
    // 处理浮点数输入框
    else if ((lineEdit == ui->LineEdit_3) || (lineEdit == ui->LineEdit_4) ||
             (lineEdit == ui->LineEdit_5) || (lineEdit == ui->lineEdit) ||
             (lineEdit == ui->lineEdit_3) || (lineEdit == ui->lineEdit_7) ||
             (lineEdit == ui->lineEdit_8)) {
        bool ok;
        float value = text.toFloat(&ok);
        if (!text.isEmpty() && ok) {
            lineEdit->setText(text);
            // 根据不同的控件，发送对应的浮点数写寄存器信号
            if (lineEdit == ui->LineEdit_3)  { emit WriteHoldFloatRegisters_TiaoShi(72, value); }
            else if (lineEdit == ui->LineEdit_4)  { emit WriteHoldFloatRegisters_TiaoShi(74, value); }
            else if (lineEdit == ui->LineEdit_5)  { emit WriteHoldFloatRegisters_TiaoShi(76, value); }
            else if (lineEdit == ui->lineEdit)    { emit WriteHoldFloatRegisters_TiaoShi(100, value); }
            else if (lineEdit == ui->lineEdit_3)  { emit WriteHoldFloatRegisters_TiaoShi(102, value); }
            else if (lineEdit == ui->lineEdit_7)  { emit WriteHoldFloatRegisters_TiaoShi(98, value); }
            else if (lineEdit == ui->lineEdit_8)  { emit WriteHoldFloatRegisters_TiaoShi(104, value); emit ReadHoldRegisters_TiaoShi(104, 2); }
        } else {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需浮点数)!");
            lineEdit->setFocus();
        }
    }
    // 注释掉的密码设置代码（暂未使用）
    //    else if(focusedWidget == ui->lineEdit)
    //    {
    //            QString newPassword = ui->lineEdit->text();
    //            ui->lineEdit->clear();
    //            ui->lineEdit->clearFocus();
    //            if((!(newPassword.isEmpty())))
    //            {
    //              //emit screen3_writeSingleRegister(3,0x01,150,newPassword.toInt());
    //              ui->lineEdit->setPlaceholderText(newPassword);
    //            }
    //            else
    //            {
    //                //my_mainwidget->Modbus_informhandle("采样时间设置范围错误!");
    //                ui->lineEdit->setFocus();
    //            }
    //    }
}

/**
 * @brief 以下为各个调试按钮的槽函数
 *
 * 每个按钮对应一个 Modbus 线圈（Coil），通过读取按钮的“state”属性判断当前状态，
 * 然后发送 WriteSingleCoil 信号以切换线圈状态（ON -> OFF 或 OFF -> ON）。
 */

void Tiao_Shi_Screen::on_pushButton_7_clicked()
{
    QString state = ui->pushButton_7->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(2, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(2, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_8_clicked()
{
    QString state = ui->pushButton_8->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(5, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(5, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_9_clicked()
{
    QString state = ui->pushButton_9->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(6, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(6, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_10_clicked()
{
    QString state = ui->pushButton_10->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(0, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(0, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_11_clicked()
{
    QString state = ui->pushButton_11->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(12, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(12, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_12_clicked()
{
    QString state = ui->pushButton_12->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(1, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(1, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_13_clicked()
{
    QString state = ui->pushButton_13->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(7, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(7, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_14_clicked()
{
    QString state = ui->pushButton_14->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(13, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(13, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_15_clicked()
{
    QString state = ui->pushButton_15->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(14, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(14, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_16_clicked()
{
    QString state = ui->pushButton_16->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(3, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(3, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_17_clicked()
{
    QString state = ui->pushButton_17->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(8, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(8, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_18_clicked()
{
    QString state = ui->pushButton_18->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(11, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(11, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_19_clicked()
{
    QString state = ui->pushButton_19->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(4, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(4, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_20_clicked()
{
    QString state = ui->pushButton_20->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(9, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(9, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_21_clicked()
{
    QString state = ui->pushButton_21->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(10, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(10, true);
    }
}

void Tiao_Shi_Screen::on_pushButton_22_clicked()
{
    QString state = ui->pushButton_22->property("state").toString();
    if (state == "ON") {
        emit WriteSingleCoil_TiaoShi(15, false);
    } else if (state == "OFF") {
        emit WriteSingleCoil_TiaoShi(15, true);
    }
}

/**
 * @brief 以下为调试界面中的功能按钮槽函数
 *
 * 这些按钮直接发送 Modbus 写寄存器命令，执行特定操作（如步进电机控制、系统复位等）。
 */

void Tiao_Shi_Screen::on_pushButton_23_clicked()
{
    // 步进电机正转 800 步
    emit WriteHoldRegisters_TiaoShi(46, 800);
    emit WriteHoldRegisters_TiaoShi(0, 12);
}

void Tiao_Shi_Screen::on_pushButton_24_clicked()
{
    // 步进电机反转 800 步
    emit WriteHoldRegisters_TiaoShi(46, -800);
    emit WriteHoldRegisters_TiaoShi(0, 12);
}

void Tiao_Shi_Screen::on_pushButton_35_clicked()
{
    // 系统复位（命令码 108）
    emit WriteHoldRegisters_TiaoShi(0, 108);
}

void Tiao_Shi_Screen::on_pushButton_36_clicked()
{
    // 停止测试（命令码 16）
    emit WriteHoldRegisters_TiaoShi(0, 16);
}

void Tiao_Shi_Screen::on_pushButton_37_clicked()
{
    // 启动水样测试（命令码 17）
    emit WriteHoldRegisters_TiaoShi(0, 17);
}

void Tiao_Shi_Screen::on_pushButton_38_clicked()
{
    // 启动标样核查（命令码 18）
    emit WriteHoldRegisters_TiaoShi(0, 18);
}

void Tiao_Shi_Screen::on_pushButton_42_clicked()
{
    // 启动零点核查（命令码 32）
    emit WriteHoldRegisters_TiaoShi(0, 32);
}

void Tiao_Shi_Screen::on_pushButton_44_clicked()
{
    // 启动跨度核查（命令码 33）
    emit WriteHoldRegisters_TiaoShi(0, 33);
}

void Tiao_Shi_Screen::on_pushButton_46_clicked()
{
    // 启动空白测试（命令码 31）
    emit WriteHoldRegisters_TiaoShi(0, 31);
}

void Tiao_Shi_Screen::on_pushButton_48_clicked()
{
    // 启动初始化（清洗）（命令码 34）
    emit WriteHoldRegisters_TiaoShi(0, 34);
}
