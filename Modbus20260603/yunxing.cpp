#include "yunxing.h"
#include "ui_yunxing.h"
#include <QLabel>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include "mainwindow.h"

Yun_Xing_Screen::Yun_Xing_Screen(QWidget *parent) :
    QWidget(parent),ui(new Ui::yunxing),my_mainwidget(static_cast<MainWindow*>(parent))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    formula = "  x²";
    keyboard_yunxing=new DigitalKeyboardcpp(ui->widget_7);
    connect(keyboard_yunxing, &DigitalKeyboardcpp::OkKeyPressed, this, &Yun_Xing_Screen::Keypressed_handle_YunXing);

    setTabletKongJian(LC_line1,LC_Label1,LC_PushButton1,ui->tableWidget);
    setTabletKongJian(LC_line2,LC_Label2,LC_PushButton2,ui->tableWidget_2);
    setTabletKongJian(LC_line3,LC_Label3,LC_PushButton3,ui->tableWidget_3);

    setAllCellsCentered(ui->tableWidget);
    setAllCellsCentered(ui->tableWidget_2);
    setAllCellsCentered(ui->tableWidget_3);
    setAllCellsCentered(ui->tableWidget_5);
    for (int i = 0; i < ui->gridLayout->count(); ++i)
    {
        QLayoutItem *item = ui->gridLayout->itemAt(i);
        if (item)
        {
            QWidget *widget = item->widget();
            if (qobject_cast<QPushButton*>(widget))
            {
                QSizePolicy policy = widget->sizePolicy();
                policy.setVerticalPolicy(QSizePolicy::Expanding);
                widget->setSizePolicy(policy);
            }
        }
    }
    ui->tableWidget->horizontalHeader()->setSectionsClickable(false);//水平表头不可点击
    ui->tableWidget->verticalHeader()->setSectionsClickable(false);//垂直表头不可点击
    ui->tableWidget_2->horizontalHeader()->setSectionsClickable(false);//水平表头不可点击
    ui->tableWidget_2->verticalHeader()->setSectionsClickable(false);//垂直表头不可点击
    ui->tableWidget_3->horizontalHeader()->setSectionsClickable(false);//水平表头不可点击
    ui->tableWidget_3->verticalHeader()->setSectionsClickable(false);//垂直表头不可点击
    ui->tableWidget_5->horizontalHeader()->setSectionsClickable(false);//水平表头不可点击
    ui->tableWidget_5->verticalHeader()->setSectionsClickable(false);//垂直表头不可点击
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget_2->horizontalHeader()->setVisible(true);
    ui->tableWidget_3->horizontalHeader()->setVisible(true);
    ui->tableWidget_5->horizontalHeader()->setVisible(true);
    SetGongZuoQuXianTab();
    connect(LC_PushButton1[0],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton10Handle);
    connect(LC_PushButton1[1],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton11Handle);
    connect(LC_PushButton1[2],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton12Handle);
    connect(LC_PushButton1[3],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton13Handle);
    connect(LC_PushButton1[4],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton14Handle);
    connect(LC_PushButton2[0],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton10Handle);
    connect(LC_PushButton2[1],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton11Handle);
    connect(LC_PushButton2[2],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton12Handle);
    connect(LC_PushButton2[3],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton13Handle);
    connect(LC_PushButton2[4],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton14Handle);
    connect(LC_PushButton3[0],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton10Handle);
    connect(LC_PushButton3[1],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton11Handle);
    connect(LC_PushButton3[2],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton12Handle);
    connect(LC_PushButton3[3],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton13Handle);
    connect(LC_PushButton3[4],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton14Handle);
    connect(LC_PushButton5[0],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton50Handle);
    connect(LC_PushButton5[1],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton51Handle);
    connect(LC_PushButton5[2],&QPushButton::clicked,this,&Yun_Xing_Screen::LC_PushButton52Handle);
    my_mainwidget->SetPushButton(false,ui->pushButton_60);
    emit WriteHoldRegisters_YunXing(215,14);
}

Yun_Xing_Screen::~Yun_Xing_Screen()
{
    delete ui;
}

void Yun_Xing_Screen::setAllCellsCentered(QTableWidget *table)
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

void Yun_Xing_Screen::Keypressed_handle_YunXing(QString text,QWidget* focusedWidget)
{
    if (!focusedWidget) return;
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(focusedWidget);

//    // 处理整数型输入框
    if ((lineEdit == ui->lineEdit_28)||(lineEdit==ui->lineEdit_63)||(lineEdit==ui->lineEdit_64)||
            (lineEdit==ui->lineEdit_50)||(lineEdit==ui->lineEdit_52)||(lineEdit==ui->lineEdit_51)
            ||(lineEdit==ui->lineEdit_53)||(lineEdit==ui->lineEdit_54)||(lineEdit==ui->lineEdit_55)||
            (lineEdit==ui->lineEdit_56)||(lineEdit==ui->lineEdit_57)||(lineEdit==ui->lineEdit_58)
            ||(lineEdit==ui->lineEdit_61)||(lineEdit==ui->lineEdit_62)||(lineEdit==ui->lineEdit_59)
            ||(lineEdit==ui->lineEdit_66)) {
        bool ok;
        uint16_t value = text.toUShort(&ok);
        // 验证：非空、纯数字、值 ≥1、转换成功
        if (!text.isEmpty() && text.contains(QRegularExpression("^\\d+$")) && value >= 1 && ok) {
            lineEdit->setText(text);
            // 根据不同的控件，发送对应的 Modbus 写寄存器信号
            if(lineEdit==ui->lineEdit_28){emit WriteHoldRegisters_YunXing(18,value);}
            else if(lineEdit==ui->lineEdit_63){emit WriteHoldRegisters_YunXing(150,value);}
            else if(lineEdit==ui->lineEdit_64){emit WriteHoldRegisters_YunXing(207,value);}
            else if(lineEdit==ui->lineEdit_50){emit WriteHoldRegisters_YunXing(178,value);}
            else if(lineEdit==ui->lineEdit_52){emit WriteHoldRegisters_YunXing(179,value);}
            else if(lineEdit==ui->lineEdit_51){emit WriteHoldRegisters_YunXing(180,value);}
            else if(lineEdit==ui->lineEdit_53){emit WriteHoldRegisters_YunXing(166,value);}
            else if(lineEdit==ui->lineEdit_54){emit WriteHoldRegisters_YunXing(167,value);}
            else if(lineEdit==ui->lineEdit_55){emit WriteHoldRegisters_YunXing(169,value);}
            else if(lineEdit==ui->lineEdit_56){emit WriteHoldRegisters_YunXing(170,value);}
            else if(lineEdit==ui->lineEdit_57){emit WriteHoldRegisters_YunXing(189,value);}
            else if(lineEdit==ui->lineEdit_58){emit WriteHoldRegisters_YunXing(188,value);}
            else if(lineEdit==ui->lineEdit_59){emit WriteHoldRegisters_YunXing(190,value);}
            else if(lineEdit==ui->lineEdit_61){emit WriteHoldRegisters_YunXing(197,value);}
            else if(lineEdit==ui->lineEdit_62){emit WriteHoldRegisters_YunXing(196,value);}
            else if(lineEdit==ui->lineEdit_66){emit WriteHoldRegisters_YunXing(202,value);}
        } else {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需整数)!");
            lineEdit->setFocus();
        }
    }
    // 处理浮点数输入框
    else if ((lineEdit == LC_line1)||(lineEdit == LC_line2)||(lineEdit==LC_line3)||
             (lineEdit==ui->lineEdit_49)||(lineEdit==ui->lineEdit_65)) {
        bool ok;
        float value = text.toFloat(&ok);
        if (!text.isEmpty() && ok) {
            lineEdit->setText(text);
            // 根据不同的控件，发送对应的浮点数写寄存器信号
            if (lineEdit == LC_line1)  { emit WriteHoldFloatRegisters_YunXing(209, value); }
            else if (lineEdit == LC_line2)  { emit WriteHoldFloatRegisters_YunXing(211, value); }
            else if (lineEdit == LC_line3)  { emit WriteHoldFloatRegisters_YunXing(213, value); }
            else if (lineEdit == ui->lineEdit_49)  { emit WriteHoldFloatRegisters_YunXing(182, value); }
            else if (lineEdit == ui->lineEdit_65)  { emit WriteHoldFloatRegisters_YunXing(198, value); }
        } else {
            my_mainwidget->InformPlayHandle("输入数据类型设置错误(需浮点数)!");
            lineEdit->setFocus();
        }
    }
}

void Yun_Xing_Screen::LC_PushButton10Handle()
{
    emit WriteHoldRegisters_YunXing(131,24);
}

void Yun_Xing_Screen::LC_PushButton11Handle()
{
    emit WriteHoldRegisters_YunXing(131,25);
}

void Yun_Xing_Screen::LC_PushButton12Handle()
{
    emit WriteHoldRegisters_YunXing(131,21);
}

void Yun_Xing_Screen::LC_PushButton13Handle()
{
    emit WriteHoldRegisters_YunXing(131,19);
}

void Yun_Xing_Screen::LC_PushButton14Handle()
{
    emit WriteHoldRegisters_YunXing(131,19);
}

void Yun_Xing_Screen::LC_PushButton50Handle()
{
    emit WriteHoldRegisters_YunXing(133,1);
}

void Yun_Xing_Screen::LC_PushButton51Handle()
{
    emit WriteHoldRegisters_YunXing(133,2);
}

void Yun_Xing_Screen::LC_PushButton52Handle()
{
    emit WriteHoldRegisters_YunXing(133,3);
}

void Yun_Xing_Screen::setTabletKongJian(QLineEdit *&line, QVector<QLabel *>& labellist, QVector<QPushButton *>& pushbuttonlist, QTableWidget *tab)
{
    line=new QLineEdit(this);
    line->setStyleSheet("background: rgb(114, 159, 207);color: rgb(0, 0, 0);border-radius: 6px;");
    line->setAlignment(Qt::AlignCenter);
    tab->setCellWidget(1,1,line);
    for(int i=0;i<3;i++){
        QLabel *label=new QLabel(this);
        label->setStyleSheet("background: transparent;color: rgb(0, 0, 0);");
        label->setAlignment(Qt::AlignCenter);
        labellist.append(label);
        if(i==2){tab->setCellWidget(1,2,label);}
        else{tab->setCellWidget(0,i+1,label);}}
    for(int i=0;i<5;i++){
        QPushButton *push=new QPushButton(this);
        push->setStyleSheet("QPushButton {background: rgb(138, 226, 52);color: rgb(0, 0, 0);border-radius: 6px;}"
                        "QPushButton:pressed {background: rgb(78, 154, 6);padding-top: 11px;padding-bottom: 9px;}");
        if(i==0){push->setText("零点校准");tab->setCellWidget(0,3,push);}
        else if(i==1){push->setText("量程校准");tab->setCellWidget(1,3,push);}
        else if(i==2){push->setText("两点校准\n转自动水样");tab->setCellWidget(0,4,push);}
        else if(i==3){push->setText("两点校准\n转自动核查");tab->setCellWidget(1,4,push);}
        else if(i==4){push->setText("两点校准\n转手动核查");tab->setCellWidget(0,5,push);}
        pushbuttonlist.append(push);
    }
    for(int i=0;i<2;i++)
    {
        QLabel *label1=new QLabel(this);
        label1->setStyleSheet("background: transparent;color: rgb(0, 0, 0);");
        label1->setAlignment(Qt::AlignCenter);
        if(i==0){label1->setText("零点");tab->setCellWidget(0,0,label1);}
        else if(i==1){label1->setText("量程");tab->setCellWidget(1,0,label1);}}
}

void Yun_Xing_Screen::SetGongZuoQuXianTab()
{
    ui->tableWidget_5->setSpan(0, 0, 3, 1);
    QLabel *line=new QLabel(this);
    line->setStyleSheet("color: rgb(0, 0, 0);font: 12pt \"Ubuntu\";font-weight: bold; ");
    line->setAlignment(Qt::AlignCenter);
    LC_Label5.append(line);
    ui->tableWidget_5->setCellWidget(0,0,line);
    for(int i=0;i<9;i++){
        QLabel *label=new QLabel(this);
        label->setStyleSheet("background: transparent;color: rgb(0, 0, 0);");
        label->setAlignment(Qt::AlignCenter);
        LC_Label5.append(label);
        ui->tableWidget_5->setCellWidget(i%3,i/3+2,label);}
    for(int i=0;i<3;i++){
        QPushButton *push=new QPushButton(this);
        push->setStyleSheet("QPushButton {background: rgb(138, 226, 52);color: rgb(0, 0, 0);border-radius: 6px;}"
                        "QPushButton:pressed {background: rgb(78, 154, 6);padding-top: 11px;padding-bottom: 9px;}");
        if(i==0){push->setText("量程1");ui->tableWidget_5->setCellWidget(0,1,push);}
        else if(i==1){push->setText("量程2");ui->tableWidget_5->setCellWidget(1,1,push);}
        else if(i==2){push->setText("量程3");ui->tableWidget_5->setCellWidget(2,1,push);}
        LC_PushButton5.append(push);
    }
}

void Yun_Xing_Screen::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow+1);
    switch (currentRow+1)
    {
        case (1):
            keyboard_yunxing->setParent(ui->widget_7);
            break;
        case (2):
            keyboard_yunxing->setParent(ui->widget_8);
            break;
        case (3):
            keyboard_yunxing->setParent(ui->widget_9);
            break;
        case (4):
            keyboard_yunxing->setParent(ui->widget_21);
            break;
        case (6):
            keyboard_yunxing->setParent(ui->widget_17);
            break;
        case (7):
            keyboard_yunxing->setParent(ui->widget_27);
            break;
        default:
            break;
    }
}

void Yun_Xing_Screen::on_pushButton_8_clicked()
{
    emit WriteHoldRegisters_YunXing(131,1);
}

void Yun_Xing_Screen::on_pushButton_11_clicked()
{
    emit WriteHoldRegisters_YunXing(131,1);
}

void Yun_Xing_Screen::on_pushButton_57_clicked()
{
    emit WriteHoldRegisters_YunXing(131,2);
}

void Yun_Xing_Screen::on_pushButton_58_clicked()
{
    emit WriteHoldRegisters_YunXing(131,2);
}

void Yun_Xing_Screen::on_pushButton_59_clicked()
{
    emit WriteHoldRegisters_YunXing(131,1);
}

void Yun_Xing_Screen::on_pushButton_95_clicked()
{
   emit WriteHoldRegisters_YunXing(149,2);
}

void Yun_Xing_Screen::on_pushButton_96_clicked()
{
    emit WriteHoldRegisters_YunXing(149,3);
}

void Yun_Xing_Screen::on_pushButton_98_clicked()
{
    emit WriteHoldRegisters_YunXing(149,4);
}

void Yun_Xing_Screen::on_pushButton_97_clicked()
{
    emit WriteHoldRegisters_YunXing(149,1);
}

void Yun_Xing_Screen::on_pushButton_99_clicked()
{
    emit WriteHoldRegisters_YunXing(149,1);
}

void Yun_Xing_Screen::on_checkBox_stateChanged(int arg1)
{
     bool isChecked = (arg1 == Qt::Checked);
     if (isChecked) {emit WriteHoldRegisters_YunXing(204,1);}
     else {emit WriteHoldRegisters_YunXing(204,0);}
}

void Yun_Xing_Screen::on_pushButton_103_clicked()
{
    QString state = ui->pushButton_103->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(63, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(63, true);}
}

void Yun_Xing_Screen::on_pushButton_101_clicked()
{
    QString state = ui->pushButton_101->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(62, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(62, true);}
}

void Yun_Xing_Screen::on_pushButton_104_clicked()
{
    QString state = ui->pushButton_104->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(61, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(61, true);}
}

void Yun_Xing_Screen::on_pushButton_105_clicked()
{
    QString state = ui->pushButton_105->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(60, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(60, true);}
}

void Yun_Xing_Screen::on_pushButton_111_clicked()
{
    QString state = ui->pushButton_111->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(59, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(59, true);}
}

void Yun_Xing_Screen::on_pushButton_109_clicked()
{
    QString state = ui->pushButton_109->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(58, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(58, true);}
}

void Yun_Xing_Screen::on_pushButton_112_clicked()
{
    QString state = ui->pushButton_112->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(57, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(57, true);}
}

void Yun_Xing_Screen::on_pushButton_117_clicked()
{
    QString state = ui->pushButton_117->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(56, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(56, true);}
}

void Yun_Xing_Screen::on_pushButton_119_clicked()
{
    QString state = ui->pushButton_119->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(55, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(55, true);}
}

void Yun_Xing_Screen::on_pushButton_106_clicked()
{
    QString state = ui->pushButton_106->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(54, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(54, true);}
}

void Yun_Xing_Screen::on_pushButton_102_clicked()
{
    QString state = ui->pushButton_102->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(53, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(53, true);}
}

void Yun_Xing_Screen::on_pushButton_122_clicked()
{
    QString state = ui->pushButton_122->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(52, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(52, true);}
}

void Yun_Xing_Screen::on_pushButton_118_clicked()
{
    QString state = ui->pushButton_118->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(51, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(51, true);}
}

void Yun_Xing_Screen::on_pushButton_107_clicked()
{
    QString state = ui->pushButton_107->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(50, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(50, true);}
}

void Yun_Xing_Screen::on_pushButton_113_clicked()
{
    QString state = ui->pushButton_113->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(49, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(49, true);}
}

void Yun_Xing_Screen::on_pushButton_121_clicked()
{
    QString state = ui->pushButton_121->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(48, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(48, true);}
}

void Yun_Xing_Screen::on_pushButton_114_clicked()
{
    QString state = ui->pushButton_114->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(47, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(47, true);}
}

void Yun_Xing_Screen::on_pushButton_123_clicked()
{
    QString state = ui->pushButton_123->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(46, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(46, true);}
}

void Yun_Xing_Screen::on_pushButton_108_clicked()
{
    QString state = ui->pushButton_108->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(45, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(45, true);}
}

void Yun_Xing_Screen::on_pushButton_115_clicked()
{
    QString state = ui->pushButton_115->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(44, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(44, true);}
}

void Yun_Xing_Screen::on_pushButton_100_clicked()
{
    QString state = ui->pushButton_100->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(43, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(43, true);}
}

void Yun_Xing_Screen::on_pushButton_120_clicked()
{
    QString state = ui->pushButton_120->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(42, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(42, true);}
}

void Yun_Xing_Screen::on_pushButton_110_clicked()
{
    QString state = ui->pushButton_110->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(41, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(41, true);}
}

void Yun_Xing_Screen::on_pushButton_116_clicked()
{
    QString state = ui->pushButton_116->property("state").toString();
    if (state == "ON") {emit WriteSingleCoil_YunXing(40, false);}
    else if (state == "OFF") {emit WriteSingleCoil_YunXing(40, true);}
}

void Yun_Xing_Screen::on_pushButton_128_clicked()
{
    QString state = ui->pushButton_128->property("state").toString();
    if (state == "ON") {emit WriteHoldRegisters_YunXing(168, false);}
    else if (state == "OFF") {emit WriteHoldRegisters_YunXing(168, true);}
}

void Yun_Xing_Screen::on_pushButton_129_clicked()
{
    QString state = ui->pushButton_129->property("state").toString();
    if (state == "ON") {emit WriteHoldRegisters_YunXing(168, false);}
    else if (state == "OFF") {emit WriteHoldRegisters_YunXing(168, true);}
}

void Yun_Xing_Screen::on_pushButton_130_clicked()
{
    QString state = ui->pushButton_130->property("state").toString();
    if (state == "ON") {emit WriteHoldRegisters_YunXing(171, false);}
    else if (state == "OFF") {emit WriteHoldRegisters_YunXing(171, true);}
}

void Yun_Xing_Screen::on_pushButton_132_clicked()
{
    QString state = ui->pushButton_132->property("state").toString();
    if (state == "ON") {emit WriteHoldRegisters_YunXing(195, false);}
    else if (state == "OFF") {emit WriteHoldRegisters_YunXing(195, true);}
}

void Yun_Xing_Screen::on_pushButton_131_clicked()
{
    QString state = ui->pushButton_131->property("state").toString();
    if (state == "ON") {emit WriteHoldRegisters_YunXing(194, false);}
    else if (state == "OFF") {emit WriteHoldRegisters_YunXing(194, true);}
}

void Yun_Xing_Screen::on_pushButton_94_clicked()
{
    emit WriteHoldRegisters_YunXing(178, 0);
    emit WriteHoldRegisters_YunXing(169, 0);
    emit WriteHoldRegisters_YunXing(188, 0);
    emit WriteHoldRegisters_YunXing(196, 0);
    emit WriteHoldRegisters_YunXing(166, 0);
}

void Yun_Xing_Screen::on_pushButton_60_clicked()
{
    QString state = ui->pushButton_60->property("state").toString();
        if (state == "ON") {emit WriteHoldRegisters_YunXing(225, 0); my_mainwidget->SetPushButton(false,ui->pushButton_60);}
        else if (state == "OFF") {emit WriteHoldRegisters_YunXing(225, 1); my_mainwidget->SetPushButton(true,ui->pushButton_60);}
}

void Yun_Xing_Screen::on_pushButton_9_clicked()
{
    emit WriteHoldRegisters_YunXing(131,3);
}

void Yun_Xing_Screen::on_pushButton_12_clicked()
{
    emit WriteHoldRegisters_YunXing(131,4);
}

void Yun_Xing_Screen::on_comboBox_currentIndexChanged(int index)
{
    if(index==0){emit WriteHoldRegisters_YunXing(215,14);}
    else if(index==1){emit WriteHoldRegisters_YunXing(215,8);}
}

void Yun_Xing_Screen::on_pushButton_10_clicked()
{
    // 创建 QMessageBox 对象，设置父窗口（this）
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("确认提醒");
        msgBox.setText("您确定要中断仪器当前测试？");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
        int x = (this->width() - msgBox.sizeHint().width()) / 2;
        int y = (this->height() - msgBox.sizeHint().height()) / 2;
        msgBox.move(x, y);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
               emit WriteHoldRegisters_YunXing(132, 1);
           }
}

void Yun_Xing_Screen::on_pushButton_124_clicked()
{
    QVector<bool> states = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    emit WriteMultipleCoils_YunXing(40,states);
}

void Yun_Xing_Screen::on_pushButton_127_clicked()
{
    QVector<bool> states = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    emit WriteMultipleCoils_YunXing(40,states);
}

void Yun_Xing_Screen::on_pushButton_125_clicked()
{
    QVector<bool> states = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    emit WriteMultipleCoils_YunXing(40,states);
}

void Yun_Xing_Screen::on_pushButton_126_clicked()
{
    QVector<bool> states = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    emit WriteMultipleCoils_YunXing(40,states);
}

void Yun_Xing_Screen::on_pushButton_134_clicked()
{
    emit WriteHoldRegisters_YunXing(208,0);
}

void Yun_Xing_Screen::on_pushButton_135_clicked()
{
    emit WriteHoldRegisters_YunXing(208,0);
}

void Yun_Xing_Screen::on_pushButton_133_clicked()
{
    emit WriteHoldRegisters_YunXing(208,0);
}
