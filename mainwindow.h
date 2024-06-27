#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qserialport.h"
#include <QTextEdit>
#include <QMessageBox>
#include <QTime>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int component_init();

private slots:
    int set_serial_bund(int index);
    int set_serial_databits(int index);
    int set_serial_stopbit(int index);
    int set_serial_check(int index);
    int set_serial_port(const QString &portname);

    int read_data();


    void on_btn_serial_switch_clicked();
    void on_btn_serial_send_clicked();
    void on_btn_clear_textSend_clicked();



private:
    Ui::MainWindow *ui;
    qint32 m_serial_bund;
    QSerialPort::DataBits m_serial_databits;
    QSerialPort::StopBits m_serial_stopbit;
    QSerialPort::Parity m_serial_check;
    QSerialPort m_serial_port;

    int check_end=0;
    QString result_str="";
    QString Reading_Str;
    quint8 start_record_sign = 0; // 0:start, 1: pause, 2: stop
    quint8 read_cmd_sign = 0;
    unsigned short record_interval = 60;
    int record_time = 60, total_record_time = 3600;
    int interval_counter = 0;
    quint8 total_time_chg_sign = 0, total_point_chg_sign = 0, pause_resume_sign = 0;
    int total_point = 60, left_point_counter = 60;
    double *power_data = nullptr;
    double laser_power_value;
    double max_power, min_power, aver_power, temp_power_sum, rmsdev_power;
    quint8 get_direct_pos_sign = 0;
};
#endif // MAINWINDOW_H
