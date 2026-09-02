#include "dengru.h"
#include "ui_dengru.h"
#include "mainwindow.h"
#include <QComboBox>
#include <QListView>

Deng_Lv_Screen::Deng_Lv_Screen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Dengru),my_mainwidget(static_cast<MainWindow*> (parent))
{
    ui->setupUi(this);
    keyboard_dengru=new DigitalKeyboardcpp(ui->widget_2);
    connect(keyboard_dengru, &DigitalKeyboardcpp::OkKeyPressed, this, &Deng_Lv_Screen::Keypressed_handl_Dengru);
    ui->ComboBox->setCurrentIndex(0);
    ui->ComboBox->setView(new QListView());
    my_mainwidget->YH_QuanXian=0;
}

Deng_Lv_Screen::~Deng_Lv_Screen()
{
    delete ui;
}

void Deng_Lv_Screen::Keypressed_handl_Dengru(QString text,QWidget* target)
{

}

void Deng_Lv_Screen::on_ComboBox_currentIndexChanged(int index)
{
    my_mainwidget->YH_QuanXian=index;
}
