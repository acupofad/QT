#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->com_serial_rate, &QComboBox::currentIndexChanged, this, [this](int index){ set_serial_bund(index); });
    connect(ui->com_serial_databits, &QComboBox::currentIndexChanged, this, [this](int index){ set_serial_databits(index); });
    connect(ui->com_serial_port, &QComboBox::currentTextChanged, this, [this](const QString &portname){ set_serial_port(portname); });
    connect(ui->com_serial_stopbit, &QComboBox::currentIndexChanged, this, [this](int index){ set_serial_stopbit(index); });
    connect(ui->com_serial_check, &QComboBox::currentIndexChanged, this, [this](int index){ set_serial_check(index); });
    connect(&m_serial_port, &QSerialPort::readyRead, this, [this](){ read_data(); });
    pTimerSend.setTimerType(Qt::PreciseTimer);
    pTimerRecv.setTimerType(Qt::PreciseTimer);
    pTimerRecv.setSingleShot(true);
    connect(&pTimerSend, &QTimer::timeout, this, &MainWindow::read_data);
    connect(&pTimerRecv, &QTimer::timeout, this, &MainWindow::slot_com_timeout_send);
    component_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::component_init()
{
    foreach(QSerialPortInfo serialInfo, QSerialPortInfo::availablePorts()){
        ui->com_serial_port->addItem(serialInfo.portName());
    }
    ui->com_serial_rate->addItems({"115200","57600","38400","9600"});
    ui->com_serial_stopbit->addItems({"1","1.5","2"});
    ui->com_serial_databits->addItems({"5","6","7","8"});
    ui->com_serial_databits->setCurrentIndex(3);
    ui->com_serial_check->addItems({"无校验","奇校验","偶校验"});
    ui->btn_serial_switch->setText("打开串口");
    ui->btn_serial_switch->setIcon(QIcon(":/img/open.png"));
    // ui->btn_serial_switch->setIconSize(QSize(100,100));
    ui->Detector_sensitivity->setRange(1,31);
    // ui->label_recv_data->setStyleSheet("background-color:black;color:#FFD700;");
    return 0;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    m_serial_port.write(ui->textEdit->toPlainText().toLatin1());
    send_count++;
    QString str;
    str.setNum(send_count);
    ui->label_send_count->setText(QString(str));
    // char temp_char[8];
    // QString temp_str;
    // sprintf(temp_char, "#SI:%02d", ui->Detector_sensitivity->value());
    // temp_str = temp_char;
    // temp_str = temp_str + "%";
    // m_serial_port.write(temp_str.toUtf8());
    m_serial_port.write("#?data%");
}

int MainWindow::set_serial_bund(int index)
{
    switch(index){
        case 0: m_serial_bund = 115200; break;
        case 1: m_serial_bund = 57600; break;
        case 2: m_serial_bund = 38400; break;
        case 3: m_serial_bund = 9600; break;
    }
    qDebug()<<"bund set to"<<m_serial_bund;
    return m_serial_port.setBaudRate(m_serial_bund);
}

int MainWindow::set_serial_databits(int index)
{
    switch(index){
        case 0: m_serial_databits = QSerialPort::Data5; break;
        case 1: m_serial_databits = QSerialPort::Data6; break;
        case 2: m_serial_databits = QSerialPort::Data7; break;
        case 3: m_serial_databits = QSerialPort::Data8; break;
    }
    qDebug()<<"databits set to"<<m_serial_databits;
    return m_serial_port.setDataBits(m_serial_databits);
}

int MainWindow::set_serial_stopbit(int index)
{
    switch(index){
        case 0: m_serial_stopbit = QSerialPort::OneStop; break;
        case 1: m_serial_stopbit = QSerialPort::OneAndHalfStop; break;
        case 2: m_serial_stopbit = QSerialPort::TwoStop; break;
    }
    qDebug()<<"stopbit set to"<<m_serial_stopbit;
    return m_serial_port.setStopBits(m_serial_stopbit);
}

int MainWindow::set_serial_check(int index)
{
    switch(index){
        case 0: m_serial_check = QSerialPort::NoParity; break;
        case 1: m_serial_check = QSerialPort::OddParity; break;
        case 2: m_serial_check = QSerialPort::EvenParity; break;
    }
    qDebug()<<"check set to"<<m_serial_check;
    return m_serial_port.setParity(m_serial_check);
}

int MainWindow::set_serial_port(const QString &portname)
{
    qDebug()<<"port set to"<<portname;
    m_serial_port.setPortName(portname);
    return 0;
}


void MainWindow::read_data()
{
    pTimerRecv.start(50);
}

void MainWindow::send_cycle()
{
    m_timer_id = startTimer(500);
}

void MainWindow::on_btn_serial_switch_clicked()
{
    if(m_serial_port.isOpen()){
        qDebug()<<"串口已关闭";
        m_serial_port.close();
        ui->btn_serial_switch->setText("打开串口");
        ui->btn_serial_switch->setIcon(QIcon(":/img/open.png"));
        // 随便设置CSS样式
        // ui->btn_serial_switch->setStyleSheet("background-color:#ffffff");

    }else{
        qDebug()<<"串口打开成功";
        m_serial_port.setFlowControl(QSerialPort::NoFlowControl);
        if(false==m_serial_port.open(QIODeviceBase::ReadWrite)){
            QMessageBox::warning(this, "错误","串口打开失败！");
        }else{
            ui->btn_serial_switch->setText("关闭串口");
            ui->btn_serial_switch->setIcon(QIcon(":/img/close.png"));
        }
    }
}

void MainWindow::on_btn_serial_send_clicked()
{
    if(m_serial_port.isOpen()){
        if(ui->checkBox_timer_send->isChecked()){
            send_cycle();
        }else{
            m_serial_port.write(ui->textEdit->toPlainText().toLatin1());
            send_count++;
            QString str;
            str.setNum(send_count);
            ui->label_send_count->setText(QString(str));
        }
    }else{
        QMessageBox::warning(this, "错误","请先打开串口！");
    }
}

void MainWindow::on_btn_clear_textSend_clicked()
{
    ui->textEdit->setText("");
}

void MainWindow::on_btn_serial_stop_clicked()
{
    killTimer(m_timer_id);
}

void MainWindow::cal_beam_pos()
{
    QString temp_str;
    temp_str = Reading_Str; // Assuming Reading_Str is a QString
    int i, k = 0;
    QVector<int> vol_data(4, 0);
    double xx, yy, temp_total;

    ui->label_14->setText(temp_str);

    if (int_pos_sign == 0)
    {
        if (get_direct_pos_sign == 1)
            int_pos_sign = 1;

        for (i = 0; i < 12; i++)
        {
            if (temp_str[i] > QChar('0' + 15) || temp_str[i] < '0')
                return;
        }

        for (k = 0; k < 12; k += 3)
        {
            vol_data[k / 3] = (temp_str[k].toLatin1() - '0') * 256 + (temp_str[k + 1].toLatin1() - '0') * 16 + (temp_str[k + 2].toLatin1() - '0');
        }

        ui->label_vol1->setText(QString::number(vol_data[0]));
        ui->progressBar_vol1->setValue(vol_data[0]);

        ui->label_vol2->setText(QString::number(vol_data[1]));
        ui->progressBar_vol2->setValue(vol_data[1]);

        ui->label_vol3->setText(QString::number(vol_data[2]));
        ui->progressBar_vol3->setValue(vol_data[2]);

        ui->label_vol4->setText(QString::number(vol_data[3]));
        ui->progressBar_vol4->setValue(vol_data[3]);

        temp_total = vol_data[0] + vol_data[1] + vol_data[2] + vol_data[3];
        if (temp_total < 10)
            xx = 0;
        else
            xx = (vol_data[1] + vol_data[3] - vol_data[0] - vol_data[2]) * 1.0 / temp_total;
        xx = (xx + 1) * 50;

        if (temp_total < 10)
            yy = 0;
        else
            yy = (vol_data[0] + vol_data[1] - vol_data[2] - vol_data[3]) * 1.0 / temp_total;
        yy = (yy + 1) * 50;

        ui->label_xPos->setText(QString::number(xx, 'f', 2));
        ui->label_yPos->setText(QString::number(yy, 'f', 2));
    }
    else if (int_pos_sign == 1)
    {
        int_pos_sign = 0;
        for (i = 0; i < 6; i++)
        {
            if (temp_str[i] > QChar('0' + 15) || temp_str[i] < '0')
                return;
        }
        for (k = 0; k < 6; k += 3)
        {
            vol_data[k / 3] = (temp_str[k].toLatin1() - '0') * 256 + (temp_str[k + 1].toLatin1() - '0') * 16 + (temp_str[k + 2].toLatin1() - '0');
        }
    }

}

void MainWindow::onReadingTimerTimeout()
{
    if (read_cmd_sign == 0)
    {
        read_cmd_sign = 1;
        Reading_Str = "";

        if (int_pos_sign == 0)
            m_serial_port.write("#?data%");
        else
            m_serial_port.write("#?pos%");
    }
    else
    {
        read_cmd_sign = 0;
        cal_beam_pos();
    }
}

void MainWindow::On1Click()
{
    get_direct_pos_sign = 1;
}

void MainWindow::Off1Click()
{
    get_direct_pos_sign = 0;

}

void MainWindow::slot_com_timeout_send()
{
    QByteArray arr = m_serial_port.readAll();
    QString string = QString::fromLatin1(arr);
    Reading_Str = arr;
    qDebug() << "Data received:" << Reading_Str;
    cal_beam_pos();
}







