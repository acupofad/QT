#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qserialport.h"
#include <QTextEdit>
#include <QMessageBox>
#include <QTimer>
#include <QThread>


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
    void timerEvent(QTimerEvent *event);

private slots:
    int set_serial_bund(int index);
    int set_serial_databits(int index);
    int set_serial_stopbit(int index);
    int set_serial_check(int index);
    int set_serial_port(const QString &portname);

    void read_data();

    void send_cycle();
    void on_btn_serial_switch_clicked();
    void on_btn_serial_send_clicked();
    void on_btn_clear_textSend_clicked();



    void on_btn_serial_stop_clicked();
    void cal_beam_pos();
    void onReadingTimerTimeout();
    void On1Click();
    void Off1Click();
    void slot_com_timeout_send();

private:
    Ui::MainWindow *ui;
    qint32 m_serial_bund;
    QSerialPort::DataBits m_serial_databits;
    QSerialPort::StopBits m_serial_stopbit;
    QSerialPort::Parity m_serial_check;
    QSerialPort m_serial_port;
    QTimer pTimerSend;
    QTimer pTimerRecv;

    int m_timer_id;
    int delay_timer_id;
    int send_count = 0;
    quint8 int_pos_sign = 0;
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
    quint8 get_direct_pos_sign = 1;
};
#endif // MAINWINDOW_H
