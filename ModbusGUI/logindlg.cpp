#include "logindlg.h"
#include "ui_logindlg.h"
#include "QMessageBox"
#include "QString"
#include "QHBoxLayout"
#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief 登录对话框构造函数
 * @param dbManager 数据库管理器引用（用于读取密码）
 * @param parent    父窗口，通常为 MainWindow
 *
 * 初始化 UI，创建数字键盘，并连接键盘的确认信号到登录验证槽函数。
 */
loginDlg::loginDlg(SqliteManager &dbManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDlg),
    m_dbManager(dbManager),
    my_mainWin(qobject_cast<MainWindow*>(parent))
{
    ui->setupUi(this);
    // 创建数字键盘，并将其放置在界面的指定容器（ui->widget）中
    keyboard = new DigitalKeyboardcpp(ui->widget);
    // 连接数字键盘的确认信号到当前类的登录验证槽函数
    connect(keyboard, &DigitalKeyboardcpp::OkKeyPressed, this, &loginDlg::KeyPressedLogInDig);
}

/**
 * @brief 析构函数，释放 UI 资源
 */
loginDlg::~loginDlg()
{
    delete ui;
}

/**
 * @brief 处理数字键盘确认信号，进行密码验证
 * @param text   键盘输入的文本（密码）
 * @param target 输入焦点控件（本函数中未使用）
 *
 * 从数据库中读取预设密码（键值为"密码"的记录），与用户输入的 text 比较。
 * 若匹配，则将主窗口的列表导航切换到第一个页面（主页）；否则提示密码错误，
 * 并清空密码输入框，重新获取焦点。
 */
void loginDlg::KeyPressedLogInDig(QString text, QWidget* target)
{
    Q_UNUSED(target);
    // 查询数据库中的密码（假设查询返回的记录中第三个字段为密码值）
    if (text == m_dbManager.queryRecord("[set]", "str1", "密码").at(2).toString())
    {
        // 密码正确，切换主窗口到主页（列表第一项）
        my_mainWin->ui->listWidget->setCurrentRow(0);
    }
    else
    {
        // 密码错误，显示提示信息并清空输入框
        my_mainWin->InformPlayHandle("用户密码输入错误!");
        ui->pwdlineEdit->setFocus();
    }
    // 无论正确与否，清空密码输入框并移除焦点
    ui->pwdlineEdit->clear();
    ui->pwdlineEdit->clearFocus();
}

/**
 * @brief 重置密码按钮槽函数
 *
 * 将数据库中的密码重置为默认值 "123456"，并给出提示。
 */
void loginDlg::on_pushButton_2_clicked()
{
    QMap<QString, QVariant> setData;
    ui->pwdlineEdit->clear();                         // 清空密码输入框
    m_dbManager.deleteRecord("[set]", "str1", "密码"); // 删除旧密码记录
    setData["str1"] = "密码";
    setData["str2"] = "123456";
    m_dbManager.insertRecord("[set]", setData);       // 插入新密码记录
    my_mainWin->InformPlayHandle("密码重置成功!");
}
