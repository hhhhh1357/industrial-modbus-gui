#include "caxun.h"
#include "ui_caxun.h"
#include "mainwindow.h"
#include "QSqlQueryModel"
#include <QHeaderView>
#include <QScrollBar>
#include "QTableWidget"
#include "QTableWidget"
#include <QProcess>
#include <QMessageBox>

Ca_Xun_Screen::Ca_Xun_Screen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::screen_2),my_mainwidget(static_cast<MainWindow*> (parent))
{
    ui->setupUi(this);
    tableView_JianChe = new QTableView(this);
    model_JianChe = new QSqlQueryModel (this);
    tableView_BiaoDing = new QTableView(this);
    model_BiaoDing = new QSqlQueryModel (this);
    tableView_BaoJing = new QTableView(this);
    model_BaoJing = new QSqlQueryModel (this);
    tableView_CaoZuo = new QTableView(this);
    model_CaoZuo = new QSqlQueryModel (this);
    GuanJianCanShu1=new QLineEdit();
    GuanJianCanShu2=new QLineEdit();
    ResetDataTab=new QTimer(this);

    setupUI(tableView_JianChe, ui->widget1,model_JianChe);
    setupUI(tableView_BiaoDing, ui->widget_3,model_BiaoDing);
    setupUI(tableView_BaoJing, ui->widget_4,model_BaoJing);
    setupUI(tableView_CaoZuo, ui->widget_6,model_CaoZuo);

    keyboard_screen2=new DigitalKeyboardcpp(ui->widget_12);
    connect(keyboard_screen2, &DigitalKeyboardcpp::OkKeyPressed, this, &Ca_Xun_Screen::Keypressed_handl_Screen2);

    ui->checkBox_0->setChecked(true);

    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    setAllCellsCentered(ui->tableWidget);
    setAllCellsCentered(ui->tableWidget_2);
    setAllCellsCentered(ui->tableWidget_3);
    setAllCellsCentered(ui->tableWidget_6);
    setAllCellsCentered(ui->tableWidget_4);

    ui->stackedWidget_2->move(10,10);
    ui->tableWidget->move(10,10);
    ui->tableWidget_3->move(10,10);
    ui->tableWidget_6->move(10,10);
    ui->tableWidget_4->move(10,10);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget_3->horizontalHeader()->setVisible(true);
    ui->tableWidget_6->horizontalHeader()->setVisible(true);
    ui->tableWidget_4->horizontalHeader()->setVisible(true);

    ui->tableWidget_6->setCellWidget(14,3,GuanJianCanShu1);
    GuanJianCanShu1->setText("0.00");
    GuanJianCanShu1->setAlignment(Qt::AlignCenter);

    ui->tableWidget_4->setCellWidget(0,1,GuanJianCanShu2);
    GuanJianCanShu2->setText("0.00");
    GuanJianCanShu2->setAlignment(Qt::AlignCenter);

    ui->checkBox_0->setEnabled(false);
    ui->checkBox_1->setEnabled(false);
    ui->checkBox_3->setEnabled(false);
    ui->checkBox_4->setEnabled(false);
    ui->checkBox_5->setEnabled(false);
    ui->checkBox_6->setEnabled(false);
    ui->checkBox_7->setEnabled(false);
    ui->checkBox_8->setEnabled(false);
    ui->checkBox_9->setEnabled(false);
    ui->checkBox_10->setEnabled(false);
    ui->checkBox_11->setEnabled(false);
    ui->checkBox_12->setEnabled(false);
    ui->checkBox_13->setEnabled(false);
    ui->checkBox_14->setEnabled(false);
    ui->checkBox_15->setEnabled(false);
    ui->checkBox_16->setEnabled(false);
    ui->checkBox_17->setEnabled(false);
    ui->checkBox_18->setEnabled(false);
    ui->checkBox_19->setEnabled(false);
    ui->checkBox_20->setEnabled(false);
    ui->checkBox_21->setEnabled(false);
    ui->checkBox_22->setEnabled(false);
    ui->checkBox_23->setEnabled(false);

    // 设置表格的表头不可点击
    ui->tableWidget_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableWidget_2->verticalHeader()->setSectionsClickable(false);

    // 动态创建表格中的 QLineEdit 控件，并保存到二维向量 SinJiTiJi_line 中
    int rowCount = ui->tableWidget_2->rowCount();
    int colCount = ui->tableWidget_2->columnCount();
    SinJiTiJi_line.resize(rowCount);
    for (int row = 0; row < rowCount; ++row) {
        SinJiTiJi_line[row].resize(colCount);
        for (int col = 0; col < colCount; ++col) {
            QLabel *line = new QLabel(this);
            line->setAlignment(Qt::AlignCenter);
            line->setStyleSheet("background: transparent;color: rgb(239, 41, 41);");
            ui->tableWidget_2->setCellWidget(row, col, line);
            SinJiTiJi_line[row][col] = line;
        }
    }
    connect(ResetDataTab,&QTimer::timeout,this,&Ca_Xun_Screen::ResetDataTab_Handler);
    ResetDataTab->start(5*60*1000);
}

Ca_Xun_Screen::~Ca_Xun_Screen()
{
    delete ui;
}


void Ca_Xun_Screen::setAllCellsCentered(QTableWidget *table)
{
    QTableWidgetItem *item;
    for(int row=0;row<table->rowCount();++row)
    {
        for(int col=0;col<table->columnCount();++col)
        {
            item=table->item(row,col);
            if(item)
            {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
}

void Ca_Xun_Screen::ResetDataTab_Handler()
{
    ResetDataTab->stop();
    refreshTable_JianChe();
    QTimer::singleShot(2*60*1000, this, [this]{
        refreshTable_BiaoDing();
      });
    QTimer::singleShot(4*60*1000, this, [this]{
        refreshTable_BaoJing();
      });
    QTimer::singleShot(6*60*1000, this, [this]{
        refreshTable_CaoZuo();
        ResetDataTab->start(5*60*1000);
      });
}

void Ca_Xun_Screen::setupUI(QTableView *tableview, QWidget *widget,QSqlQueryModel *model)
{ 
   QVBoxLayout *vbox=new QVBoxLayout();
   vbox->addWidget(tableview);
   widget->setLayout(vbox);

   tableview->setModel(model);
   tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
   tableview->setAlternatingRowColors(false);
   QString styleSheet =
       "QTableView {"
       "  color: rgb(0, 0, 0);"
       "  background-color: rgb(255, 255, 255);"
       "  gridline-color: green;"
       " border: 3px solid green;"
       "}"
       "QHeaderView {"
       "  background-color: #222;"             // 表头背景
       "}"
       "QHeaderView::section {"
       "  color: white;"
       "  background-color: green;"            // 表头单元格绿色
       "  padding: 6px 10px;"                 // 增加内边距
       "  border: 1px solid #444;"            // 边框
       "  border-radius: 3px;"                // 圆角
       "  font-weight: bold;"
       "  font-size: 12pt;"                   // 字体大小
       "}"
       "QHeaderView::section:hover {"          // 悬停效果
       "  background-color: #32CD32;"          // 更亮的绿色
       "}"
       "QTableView::item {"
       "  background-color: rgb(255, 255, 255);"             // 单元格背景
       "  border-bottom: 1px solid green;"      // 底部边框
       "}"
       "QTableView::item:selected {"          // 选中项样式
       "  background-color: rgb(114, 159, 207);"
       "  color: white;"
       "}"
       "QScrollBar:vertical {"                // 垂直滚动条
       "  background: white;"
       "  width: 1px;"
       "}"
       "QScrollBar::handle:vertical {"        // 滚动条手柄
       "  background: white;"
       "  border-radius: 1px;"
       "}"
       "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {" // 滚动条箭头
       "  background: white;"
       "}";

   tableview->setStyleSheet(styleSheet);
   tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
   tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 自适应宽度
   tableview->verticalHeader()->setVisible(false);  // 隐藏行号

   tableview->viewport()->update();
   // 在创建表格后添加
   tableview->viewport()->installEventFilter(my_mainwidget);  // 关键：监视内容区域事件
   tableview->installEventFilter(my_mainwidget);              // 监视整个表格事件
}

void Ca_Xun_Screen::refreshTable_JianChe()
{
    if(!my_mainwidget->dbManager.m_db.isOpen())
    {
        my_mainwidget->dbManager.openDatabase("/opt/Data_lib/ZongLin.db");
    }
    QString queryStr = "SELECT * FROM [jianche] ORDER BY id DESC LIMIT 100";
    model_JianChe->setQuery(queryStr, my_mainwidget->dbManager.m_db);
    model_JianChe->setHeaderData(0, Qt::Horizontal, tr("序号"));
    model_JianChe->setHeaderData(1, Qt::Horizontal, tr("V1"));
    model_JianChe->setHeaderData(2, Qt::Horizontal, tr("V2"));
    model_JianChe->setHeaderData(3, Qt::Horizontal, tr("检测值"));
    model_JianChe->setHeaderData(4, Qt::Horizontal, tr("使用量程"));
    model_JianChe->setHeaderData(5, Qt::Horizontal, tr("数据类型"));
    model_JianChe->setHeaderData(6, Qt::Horizontal, tr("出值时间"));
}

void Ca_Xun_Screen::refreshTable_BiaoDing()
{
    if(!my_mainwidget->dbManager.m_db.isOpen())
    {
        my_mainwidget->dbManager.openDatabase("/opt/Data_lib/ZongLin.db");
    }
    QString queryStr = "SELECT * FROM [biaoding] ORDER BY id DESC LIMIT 100";
    model_BiaoDing->setQuery(queryStr, my_mainwidget->dbManager.m_db);
    model_BiaoDing->setHeaderData(0, Qt::Horizontal, tr("序号"));
    model_BiaoDing->setHeaderData(1, Qt::Horizontal, tr("标定类型"));
    model_BiaoDing->setHeaderData(2, Qt::Horizontal, tr("量程"));
    model_BiaoDing->setHeaderData(3, Qt::Horizontal, tr("浓度"));
    model_BiaoDing->setHeaderData(4, Qt::Horizontal, tr("标定吸光度"));
    model_BiaoDing->setHeaderData(5, Qt::Horizontal, tr("二次项"));
    model_BiaoDing->setHeaderData(6, Qt::Horizontal, tr("一次项"));
    model_BiaoDing->setHeaderData(7, Qt::Horizontal, tr("常数项"));
    model_BiaoDing->setHeaderData(8, Qt::Horizontal, tr("出值时间"));
}

void Ca_Xun_Screen::refreshTable_BaoJing()
{
    if(!my_mainwidget->dbManager.m_db.isOpen())
    {
        my_mainwidget->dbManager.openDatabase("/opt/Data_lib/ZongLin.db");
    }
    QString queryStr = "SELECT * FROM [baojing] ORDER BY id DESC LIMIT 60";
    model_BaoJing->setQuery(queryStr, my_mainwidget->dbManager.m_db);
    model_BaoJing->setHeaderData(0, Qt::Horizontal, tr("序号"));
    model_BaoJing->setHeaderData(1, Qt::Horizontal, tr("内容"));
    model_BaoJing->setHeaderData(2, Qt::Horizontal, tr("触发时间"));
}

void Ca_Xun_Screen::refreshTable_CaoZuo()
{
    if(!my_mainwidget->dbManager.m_db.isOpen())
    {
        my_mainwidget->dbManager.openDatabase("/opt/Data_lib/ZongLin.db");
    }
    QString queryStr = "SELECT * FROM [caozuo] ORDER BY id DESC LIMIT 100";
    model_CaoZuo->setQuery(queryStr, my_mainwidget->dbManager.m_db);
    model_CaoZuo->setHeaderData(0, Qt::Horizontal, tr("序号"));
    model_CaoZuo->setHeaderData(1, Qt::Horizontal, tr("操作日志"));
    model_CaoZuo->setHeaderData(2, Qt::Horizontal, tr("操作时间"));
}


void Ca_Xun_Screen::on_pushButton_14_clicked()
{
    refreshTable_JianChe();
}

void Ca_Xun_Screen::on_pushButton_11_clicked()
{
    tableView_JianChe->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void Ca_Xun_Screen::on_pushButton_10_clicked()
{
    tableView_JianChe->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void Ca_Xun_Screen::on_pushButton_9_clicked()
{
    tableView_JianChe->verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
}

void Ca_Xun_Screen::on_pushButton_12_clicked()
{
    tableView_JianChe->verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

void Ca_Xun_Screen::on_pushButton_16_clicked()
{
    tableView_BiaoDing->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void Ca_Xun_Screen::on_pushButton_15_clicked()
{
    tableView_BiaoDing->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void Ca_Xun_Screen::on_pushButton_17_clicked()
{
    tableView_BaoJing->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void Ca_Xun_Screen::on_pushButton_35_clicked()
{
    tableView_BaoJing->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void Ca_Xun_Screen::on_pushButton_36_clicked()
{
    tableView_CaoZuo->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void Ca_Xun_Screen::on_pushButton_38_clicked()
{
    tableView_CaoZuo->verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void Ca_Xun_Screen::on_listWidget_currentRowChanged(int currentRow)
{
     ui->stackedWidget->setCurrentIndex(currentRow+1);
     switch (currentRow+1) {
     case 1:
         refreshTable_JianChe();
         break;
     case 2:
         refreshTable_BiaoDing();
         break;
     case 4:
         refreshTable_BaoJing();
         break;
     case 5:
         refreshTable_CaoZuo();
         break;
     default:
         break;
     }
}

void Ca_Xun_Screen::on_listWidget_2_currentRowChanged(int currentRow)
{
    ui->stackedWidget_2->setCurrentIndex(currentRow+1);
    switch (currentRow+1)
    {
        case 3:
            keyboard_screen2->setParent(ui->widget_12);
            break;
        case 4:
            keyboard_screen2->setParent(ui->widget_13);
            break;
        default:
            break;
    }
}

void Ca_Xun_Screen::Keypressed_handl_Screen2(QString text,QWidget* focusedWidget)
{
    if (!focusedWidget) return;
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(focusedWidget);

    // 处理整数型输入框
    if (1) {
        bool ok;
        uint16_t value = text.toUShort(&ok);
        // 验证：非空、纯数字、值 ≥1、转换成功
        if (!text.isEmpty() && text.contains(QRegularExpression("^\\d+$")) && value >= 1 && ok) {
            lineEdit->setText(text);
            // 根据不同的控件，发送对应的 Modbus 写寄存器信号

        } else {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需整数)!");
            lineEdit->setFocus();
        }
    }
    // 处理浮点数输入框
    else if (1) {
        bool ok;
        float value = text.toFloat(&ok);
        if (!text.isEmpty() && ok) {
            lineEdit->setText(text);

        } else {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需浮点数)!");
            lineEdit->setFocus();
        }
    }
}

void Ca_Xun_Screen::on_pushButton_39_clicked()
{
    refreshTable_BiaoDing();
}

void Ca_Xun_Screen::on_pushButton_40_clicked()
{
    refreshTable_BaoJing();
}

void Ca_Xun_Screen::on_pushButton_41_clicked()
{
    refreshTable_CaoZuo();
}

void Ca_Xun_Screen::on_pushButton_13_clicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) btn->setEnabled(false);

    QProcess *setupProcess = new QProcess(this);
    setupProcess->setProgram("/opt/ModbusQt/bin/setup_udisk.sh");

        connect(setupProcess, &QProcess::started, this, [this, btn, setupProcess]() {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("U盘导出");
            msgBox.setText("U盘已启用，请将OTG口连接到电脑......");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::No);
            QAbstractButton *absBtn = msgBox.button(QMessageBox::No);
            QPushButton *closeBtn = qobject_cast<QPushButton*>(absBtn);
            if (closeBtn){closeBtn->setText(tr("关闭"));}
            msgBox.setMinimumSize(400, 200);   // 最小宽400，高200
            msgBox.setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
            int x = (this->width() - msgBox.sizeHint().width()) / 2;
            int y = (this->height() - msgBox.sizeHint().height()) / 2;
            msgBox.move(x, y);
            int ret = msgBox.exec();
            if (ret == QMessageBox::No) {
                QProcess *stopProcess = new QProcess(this);
                stopProcess->setProgram("/opt/ModbusQt/bin/stop_udisk.sh");
                connect(stopProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                        this, [this, &msgBox, setupProcess, stopProcess, btn](int, QProcess::ExitStatus) {
                    setupProcess->deleteLater();
                    stopProcess->deleteLater();
                    if (btn) btn->setEnabled(true);
                    my_mainwidget->InformPlayHandle("U盘已停止请拔出OTG!");
                });stopProcess->start();}});

    connect(setupProcess, &QProcess::errorOccurred, this, [this, btn](QProcess::ProcessError) {
        my_mainwidget->InformPlayHandle("启动U盘脚本失败!");if (btn) btn->setEnabled(true);});
    setupProcess->start();
}
