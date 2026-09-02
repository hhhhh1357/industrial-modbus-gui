#include "zhuye.h"
#include "ui_zhuye.h"
#include "mainwindow.h"

/**
 * @brief Zhu_Ye_Screen 构造函数
 * @param parent 父窗口指针，通常是 MainWindow 实例
 *
 * 初始化主界面（主页）的 UI 组件，并保存父窗口指针以供后续调用。
 */
Zhu_Ye_Screen::Zhu_Ye_Screen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::screen_1),               // 创建 UI 对象，基于 screen_1 界面文件
    my_mainwidget(static_cast<MainWindow*> (parent))  // 将父窗口指针转换为 MainWindow* 类型并保存
{
    ui->setupUi(this);                  // 设置 UI 界面
}

/**
 * @brief 析构函数，释放 UI 资源
 */
Zhu_Ye_Screen::~Zhu_Ye_Screen()
{
    delete ui;
}

/**
 * @brief 处理“中断测试”按钮点击事件
 *
 * 弹出确认对话框询问用户是否中断仪器当前测试：
 * - 若用户点击“是”，则发送 WriteHoldRegisters_ZhuYe 信号，
 *   参数为寄存器地址 132 和写入值 1（该信号由父窗口连接至 Modbus 写入操作）。
 * - 若用户点击“否”，则无操作。
 */
void Zhu_Ye_Screen::on_pushButton_clicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (btn) btn->setEnabled(false);
    // 创建 QMessageBox 对象，设置父窗口（this）
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("确认提醒");
        msgBox.setText("您确定要中断仪器当前测试？");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setWindowFlags(Qt::SubWindow);
        int x = (this->width() - msgBox.sizeHint().width()) / 2;
        int y = (this->height() - msgBox.sizeHint().height()) / 2;
        msgBox.move(x, y);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
               emit WriteHoldRegisters_ZhuYe(132, 1);
           }
        if (btn) btn->setEnabled(true);
}
