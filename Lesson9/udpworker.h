#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>

#define BIND_PORT 12345
#define BIND_PORT_MESSAGES 12346                                    // Новый порт для сообщений

class UDPworker : public QObject
{
    Q_OBJECT
public:
    explicit UDPworker(QObject* parent = nullptr);
    void InitSocket(void);
    void ReadDatagram(QNetworkDatagram datagram);
    void SendDatagram(QByteArray data, bool isUserMessage = false); // Новый параметр

private slots:
    void readPendingDatagrams(void);
    void readPendingUserMessages(void);                             // Новый слот для сообщений

private:
    QUdpSocket* serviceUdpSocket;                                   // Сокет для времени
    QUdpSocket* messageUdpSocket;                                   // Новый сокет для сообщений

signals:
    void sig_sendTimeToGUI(QDateTime data);
    void sig_sendMessageToGUI(QString message, QHostAddress senderAddress, qint64 size); // Новый сигнал

};

#endif // UDPWORKER_H
