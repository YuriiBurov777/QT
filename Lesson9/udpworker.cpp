#include "udpworker.h"
#include <QDataStream>

UDPworker::UDPworker(QObject* parent) : QObject(parent)
{
}

void UDPworker::InitSocket()
{
    // Инициализация сокета для времени
    serviceUdpSocket = new QUdpSocket(this);
    serviceUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT);
    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);

    // Инициализация нового сокета для пользовательских сообщений
    messageUdpSocket = new QUdpSocket(this);
    messageUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT_MESSAGES);
    connect(messageUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingUserMessages);
}

/*!
 * @brief Метод осуществляет обработку принятой датаграммы
 */
void UDPworker::ReadDatagram(QNetworkDatagram datagram)
{
    QByteArray data = datagram.data();

    QDataStream inStr(&data, QIODevice::ReadOnly);
    QDateTime dateTime;
    inStr >> dateTime;

    emit sig_sendTimeToGUI(dateTime);
}

/*!
 * @brief Метод осуществляет опередачу датаграммы
 */
void UDPworker::SendDatagram(QByteArray data, bool isUserMessage)
{
    if (isUserMessage) {
        // Отправляем сообщение на отдельный порт
        messageUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT_MESSAGES);
    }
    else {
        // Отправляем время на стандартный порт
        serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT);
    }
}

/*!
 * @brief Метод осуществляет чтение датаграм из сокета
 */
void UDPworker::readPendingDatagrams(void)
{
    while (serviceUdpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}

// Метод для чтения пользовательских сообщений
void UDPworker::readPendingUserMessages(void)
{
    while (messageUdpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = messageUdpSocket->receiveDatagram();
        QByteArray data = datagram.data();

        QDataStream inStr(&data, QIODevice::ReadOnly);
        QString message;
        inStr >> message;

        // Отправляем информацию о полученном сообщении в GUI
        emit sig_sendMessageToGUI(message, datagram.senderAddress(), data.size());
    }
}
