#ifndef SCREEN_2_H
#define SCREEN_2_H

#include <QWidget>
#include <QTableView>
#include "QSqlQueryModel"
#include "QTableWidget"
#include<QLineEdit>
#include<QTimer>
#include "digitalkeyboardcpp.h"
#include "ui_caxun.h"

class MainWindow;

namespace Ui {
class screen_2;
}

class Ca_Xun_Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Ca_Xun_Screen(QWidget *parent = nullptr);
    ~Ca_Xun_Screen();
    QLineEdit *GuanJianCanShu1;
    QLineEdit *GuanJianCanShu2;
    Ui::screen_2 *ui;
    QVector<QVector<QLabel*>> SinJiTiJi_line;

signals:
    void WriteHoldFloatRegisters_CaXun(int regAddr, float value);
    void WriteHoldRegisters_CaXun(int regAddr, uint16_t value);
    void WriteSingleCoil_CaXun(int coilAddr, bool value);

private slots:
    void on_pushButton_14_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_35_clicked();

    void on_pushButton_36_clicked();

    void on_pushButton_38_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_listWidget_2_currentRowChanged(int currentRow);

    void Keypressed_handl_Screen2(QString text,QWidget* focusedWidget);

    void on_pushButton_39_clicked();

    void on_pushButton_40_clicked();

    void on_pushButton_41_clicked();
    void ResetDataTab_Handler();

    void on_pushButton_13_clicked();

private:
    QTableView *tableView_JianChe;
    QSqlQueryModel *model_JianChe;

    QTableView *tableView_BiaoDing;
    QSqlQueryModel *model_BiaoDing;

    QTableView *tableView_BaoJing;
    QSqlQueryModel *model_BaoJing;

    QTableView *tableView_CaoZuo;
    QSqlQueryModel *model_CaoZuo;

    MainWindow *my_mainwidget;
    DigitalKeyboardcpp *keyboard_screen2;

    QTimer *ResetDataTab;

    void setupUI(QTableView *tableview, QWidget *widget,QSqlQueryModel *model);
    void refreshTable_JianChe();
    void refreshTable_BiaoDing();
    void refreshTable_BaoJing();
    void refreshTable_CaoZuo();
    void setAllCellsCentered(QTableWidget *table);

};

#endif // SCREEN_2_H
