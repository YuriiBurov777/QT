#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpWorker = new UDPworker(this);
    udpWorker->InitSocket();

    // Cигналы для времени
    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);

    // Cигнал для сообщений
    connect(udpWorker, &UDPworker::sig_sendMessageToGUI, this, &MainWindow::DisplayMessage);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&] {
        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        udpWorker->SendDatagram(dataToSend, false);
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
}

void MainWindow::DisplayTime(QDateTime data)
{
    counterPck++;
    if (counterPck % 20 == 0) {
        ui->te_result->clear();
    }

    ui->te_result->append("Текущее время: " + data.toString() + ". "
        "Принято пакетов " + QString::number(counterPck));
}

void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
}

// Метод для отправки сообщения
void MainWindow::on_pb_sendMessage_clicked()
{
    QString message = ui->le_message->text();

    if (message.isEmpty()) {
        ui->te_result->append("Ошибка: сообщение не может быть пустым");
        return;
    }

    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    // Отправляем текст сообщения
    outStr << message;

    // Отправляем датаграмму с флагом, что это пользовательское сообщение
    udpWorker->SendDatagram(dataToSend, true); // true - это пользовательское сообщение

    ui->te_result->append("Отправлено сообщение: " + message);
    ui->le_message->clear(); // Очищаем поле ввода
}

// Метод для отображения полученных сообщений
void MainWindow::DisplayMessage(QString message, QHostAddress senderAddress, qint64 size)
{
    ui->te_result->append(QString("Принято сообщение от %1, размер сообщения(байт) %2")
        .arg(senderAddress.toString())
        .arg(size));
    ui->te_result->append("Содержимое сообщения: " + message);
    ui->te_result->append("");
}
