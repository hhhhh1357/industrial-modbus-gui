#include "digitalkeyboardcpp.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QApplication>
#include "QLineEdit"
#include "QTextEdit"
#include "logindlg.h"

/**
 * @brief 数字键盘构造函数
 * @param parent 父窗口指针
 *
 * 初始化键盘界面，设置样式、最小尺寸，创建网格布局和信号映射器。
 * 创建显示输入内容的QLineEdit，并安装事件过滤器以监听全局焦点变化。
 */
DigitalKeyboardcpp::DigitalKeyboardcpp(QWidget *parent) : QWidget(parent), targetWidget(nullptr)
{
    setStyleSheet("DigitalKeyboard { background-color: #f0f0f0; border: 1px solid #ccc; }");
    setMinimumHeight(300);
    setMinimumWidth(300);

    layout = new QGridLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(5, 5, 5, 5);
    signalMapper = new QSignalMapper(this);

    // 创建顶部输入行（显示已输入的字符）
    key_lineedit = new QLineEdit(this);
    layout->addWidget(key_lineedit, 0, 0, 1, 3);
    key_lineedit->setStyleSheet(R"(
        QLineEdit {
            font-size: 20px;
            background-color:rgb(52, 101, 164);
            border-radius: 5px;
            padding: 10px;
            color: white;
        }
    )");

    // 将信号映射器的mapped信号连接到处理函数
    connect(signalMapper, QOverload<const QString &>::of(&QSignalMapper::mapped),
            this, &DigitalKeyboardcpp::handleButtonClick);

    createLayout();               // 创建键盘按钮布局
    qApp->installEventFilter(this); // 安装事件过滤器以监控焦点变化
    setupFocusPolicy();          // 设置按钮无焦点，避免干扰
}

/**
 * @brief 设置当前目标输入控件
 * @param target 目标QWidget，应为QLineEdit或QTextEdit
 *
 * 用于指定键盘输入将传递到的控件。
 */
void DigitalKeyboardcpp::setTargetWidget(QWidget *target)
{
    targetWidget = target;
}

/**
 * @brief 创建键盘按钮布局
 *
 * 依次创建数字1-9、0、负号、小数点、后退、退出、确定按钮。
 * 按钮位置通过网格布局行列指定。
 */
void DigitalKeyboardcpp::createLayout()
{
    // 数字 1-9，排列为 3x3
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        createButton(QString::number(i + 1), row + 1, col);
    }
    createButton("0", 4, 0);
    createButton("-", 4, 1);
    createButton(".", 4, 2);
    createButton("后退", 5, 0);
    createButton("退出", 5, 1);
    createButton("确定", 5, 2);
}

/**
 * @brief 创建一个按钮并添加到布局中
 * @param text    按钮显示文本
 * @param row     行索引
 * @param col     列索引
 * @param rowSpan 跨行数（默认1）
 * @param colSpan 跨列数（默认1）
 *
 * 设置按钮样式，并连接点击信号到信号映射器。
 */
void DigitalKeyboardcpp::createButton(const QString &text, int row, int col, int rowSpan, int colSpan)
{
    QPushButton *button = new QPushButton(text, this);
    button->setStyleSheet(
        "QPushButton {"
        "   font-size: 20px;"
        "   background-color: rgb(52, 101, 164);"
        "   color: white;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "min-width: 30px; "
        " min-height: 25px;"
        "}"
        "QPushButton:pressed {"
        "   background-color:rgb(138, 226, 52);;"
        "}"
    );
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(button, row, col, rowSpan, colSpan);

    // 将按钮的clicked信号映射到信号映射器，并携带按钮文本
    connect(button, &QPushButton::clicked, signalMapper, QOverload<>::of(&QSignalMapper::map));
    signalMapper->setMapping(button, text);
}

/**
 * @brief 设置所有按钮的焦点策略为 NoFocus，避免按钮获取焦点导致输入框失去焦点
 *
 * 同时将键盘自身焦点策略设为 NoFocus，确保焦点始终在输入控件上。
 */
void DigitalKeyboardcpp::setupFocusPolicy()
{
    for (QPushButton *button : findChildren<QPushButton *>()) {
        button->setFocusPolicy(Qt::NoFocus);
    }
    this->setFocusPolicy(Qt::NoFocus);
}

/**
 * @brief 事件过滤器，用于监听全局焦点变化
 * @param watched 被监视的对象
 * @param event   事件
 * @return 是否处理了事件
 *
 * 当任意控件获得焦点时，调用 onInputFocusChanged 更新目标控件并显示/隐藏键盘。
 */
bool DigitalKeyboardcpp::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        QWidget *widget = qobject_cast<QWidget *>(watched);
        if (widget) {
            this->onInputFocusChanged(nullptr, widget);
        }
    }
    return QWidget::eventFilter(watched, event);
}

/**
 * @brief 处理按钮点击事件
 * @param text 按钮文本（包括数字、符号、功能键）
 *
 * 根据按钮文本执行相应操作：
 * - 数字/符号：追加到键盘顶部的 QLineEdit 中
 * - 后退：模拟按下退格键，删除最后一个字符
 * - 退出：清除目标控件焦点，隐藏键盘
 * - 确定：发出 OkKeyPressed 信号，将输入文本传递给目标控件，然后隐藏键盘
 */
void DigitalKeyboardcpp::handleButtonClick(const QString &text)
{
    if (!targetWidget) return;
    QWidget *parentWidget = this->parentWidget();

    if (text == "后退") {
        // 模拟退格键事件，删除 key_lineedit 中的最后一个字符
        QKeyEvent *event1 = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
        QCoreApplication::postEvent(key_lineedit, event1);
        // 注：原代码中还有向 targetWidget 发送退格事件，此处注释掉，实际只删除顶部输入栏的内容
    } else if (text == "退出") {
        // 清除目标控件的焦点并隐藏键盘
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(targetWidget)) {
            lineEdit->clearFocus();
        } else if (QTextEdit *textEdit = qobject_cast<QTextEdit *>(targetWidget)) {
            textEdit->clearFocus();
        }
        this->hide();
        parentWidget->lower();           // 将键盘父窗口置于底层
        key_lineedit->clear();           // 清空顶部输入栏
    } else if (text == "确定") {
        // 清除目标控件焦点，发出信号（携带输入文本和目标控件指针），然后隐藏键盘
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(targetWidget)) {
            lineEdit->clearFocus();
        } else if (QTextEdit *textEdit = qobject_cast<QTextEdit *>(targetWidget)) {
            textEdit->clearFocus();
        }
        emit OkKeyPressed(key_lineedit->text(), targetWidget);
        this->hide();
        parentWidget->lower();
        key_lineedit->clear();
    } else {
        // 普通按键：将文本追加到顶部输入栏
        key_lineedit->setText(key_lineedit->text() + text);
    }
}

/**
 * @brief 焦点变化处理函数
 * @param old 原焦点控件（未使用）
 * @param now 新焦点控件
 *
 * 当焦点进入 QLineEdit 或 QTextEdit 时，将该控件设为目标控件，
 * 显示键盘并将其父窗口提升到最前。若焦点在 loginDlg 中的输入框，则设置输入掩码为密码模式。
 * 否则隐藏键盘。
 */
void DigitalKeyboardcpp::onInputFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)

    QWidget *parentWidget = this->parentWidget();

    // 焦点在 QLineEdit 上
    if (now && (qobject_cast<QLineEdit *>(now))) {
        this->setTargetWidget(now);
        this->show();
        parentWidget->raise();    // 将键盘父窗口提升到最前
        // 如果父窗口的父窗口是 loginDlg，则将顶部输入栏设为密码模式（隐藏输入内容）
        if (qobject_cast<loginDlg *>(this->parentWidget()->parentWidget())) {
            key_lineedit->setEchoMode(QLineEdit::Password);
        } else {
            key_lineedit->setEchoMode(QLineEdit::Normal);
        }
    }
    // 焦点在 QTextEdit 上
    else if (qobject_cast<QTextEdit *>(now)) {
        this->setTargetWidget(now);
        this->show();
        parentWidget->raise();
    }
    // 焦点在其他控件或没有焦点时，隐藏键盘
    else {
        this->hide();
        parentWidget->lower();
        key_lineedit->clear();
    }
}
