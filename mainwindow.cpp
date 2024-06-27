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
    ui->label_recv_data->setStyleSheet("background-color:black;color:#FFD700;");
    return 0;
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

int MainWindow::read_data()
{
    QByteArray arr = m_serial_port.readAll();
    Reading_Str  = ui->label_recv_data -> text();
    Reading_Str += "\r\n";
    Reading_Str += arr;
    ui->label_recv_data->setText(Reading_Str);
    qDebug() << "Data received:" << Reading_Str;
    return 1;
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
        m_serial_port.write(ui->textEdit->toPlainText().toLatin1());
    }else{
        QMessageBox::warning(this, "错误","请先打开串口！");
    }
}

void MainWindow::on_btn_clear_textSend_clicked()
{
    ui->textEdit->setText("");
}


