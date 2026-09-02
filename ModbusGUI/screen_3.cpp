#include "screen_3.h"
#include "ui_screen_3.h"
#include "mainwindow.h"
#include "QTimer"
screen_3::screen_3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::screen_3),my_mainwidget(static_cast<MainWindow*> (parent))
{
    ui->setupUi(this);
    screen3_timer=new QTimer(this);
    connect(screen3_timer, &QTimer::timeout, this, &screen_3::screen3_timer_handler);
    keyboarscreen3=new DigitalKeyboardcpp(ui->widget);
    connect(keyboarscreen3, &DigitalKeyboardcpp::OkKeyPressed, this, &screen_3::Keypressed_handle_Screen3);
    //screen3_timer->start(2000);
}

screen_3::~screen_3()
{
    delete ui;
}



void screen_3::on_comboBox_currentIndexChanged(int index)
{
    if(index==0)
    {
        emit screen3_writeSingleRegister(131,1);
    }
    else if(index==1)
    {
        emit screen3_writeSingleRegister(131,24);
    }
}

void screen_3::screen3_timer_handler()
{
    emit screen3_readHoldingRegisters(131,21);
    /*emit screen3_readHoldingRegisters(3,0x01,134,2);
    emit screen3_readHoldingRegisters(3,0x01,136,2);
    emit screen3_readHoldingRegisters(3,0x01,138,2);
    emit screen3_readHoldingRegisters(3,0x01,140,1);
    emit screen3_readHoldingRegisters(3,0x01,141,1);
    emit screen3_readHoldingRegisters(3,0x01,142,1);
    emit screen3_readHoldingRegisters(3,0x01,143,1);
    emit screen3_readHoldingRegisters(3,0x01,144,2);
    emit screen3_readHoldingRegisters(3,0x01,146,1);
    emit screen3_readHoldingRegisters(3,0x01,147,1);
    emit screen3_readHoldingRegisters(3,0x01,148,1);
    emit screen3_readHoldingRegisters(3,0x01,151,1);*/
}


void screen_3::Keypressed_handle_Screen3(QString text,QWidget* focusedWidget)
{
    if(text=="确定"){
        if(focusedWidget == ui->lineEdit)
        {
            QString newPassword = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->lineEdit->clearFocus();
            if((!(newPassword.isEmpty()))&&(newPassword.toInt()<=24)&&(newPassword.toInt()>=1))
            {
              emit screen3_writeSingleRegister(150,newPassword.toInt());
              ui->lineEdit->setPlaceholderText(newPassword);
            }
            else
            {
                my_mainwidget->InformPlayHandle("采样时间设置范围错误!");
                ui->lineEdit->setFocus();
            }
        }
    }
}

void screen_3::on_comboBox_2_currentIndexChanged(int index)
{
    emit screen3_writeSingleRegister(133,index+1);
}

void screen_3::modbusstate_chang(bool connected)
{
    if(connected==false)
    {
        screen3_timer->stop();
    }
    else if(connected==true)
    {
        screen3_timer->start(2000);
    }
}
