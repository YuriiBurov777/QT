#include "tcpclient.h"

/* ServiceHeader operators
* Для работы с потоками наши данные необходимо сериализовать.
* Поскольку типы данных не стандартные перегрузим оператор << Для работы с ServiceHeader
*/
QDataStream &operator >>(QDataStream &out, ServiceHeader &data){
    out >> data.id;
    out >> data.idData;
    out >> data.status;
    out >> data.len;
    return out;
}

QDataStream &operator <<(QDataStream &in, ServiceHeader &data){
    in << data.id;
    in << data.idData;
    in << data.status;
    in << data.len;
    return in;
}

/* Конструктор TCPclient
 * Поскольку мы являемся клиентом, инициализацию сокета
 * проведем в конструкторе. Также необходимо соединить
 * сокет со всеми необходимыми нам сигналами.
*/
TCPclient::TCPclient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    // Соединяем сигналы сокета с нашими слотами
    connect(socket, &QTcpSocket::readyRead, this, &TCPclient::ReadyReed);
    connect(socket, &QTcpSocket::connected, this, [this](){
        emit sig_connectStatus(STATUS_SUCCES);
    });
    connect(socket, &QTcpSocket::disconnected, this, [this](){
        emit sig_Disconnected();
    });
    connect(socket, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this, [this](QAbstractSocket::SocketError error){
        Q_UNUSED(error);
        emit sig_connectStatus(ERR_CONNECT_TO_HOST);
    });
}

/* Отправка запроса без данных */
void TCPclient::SendRequest(ServiceHeader head)
{
    if(socket->state() == QTcpSocket::ConnectedState){
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << head;
        socket->write(data);
    }
}

/* Отправка запроса с данными */
void TCPclient::SendData(ServiceHeader head, QString str)
{
    if(socket->state() == QTcpSocket::ConnectedState){
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        head.len = str.toUtf8().size();
        stream << head;
        stream << str;

        socket->write(data);
    }
}

/* Подключение к серверу */
void TCPclient::ConnectToHost(QHostAddress host, uint16_t port)
{
    if(socket->state() != QTcpSocket::ConnectedState){
        socket->connectToHost(host, port);
        if(!socket->waitForConnected(3000)){
            emit sig_connectStatus(ERR_CONNECT_TO_HOST);
        }
    }
}

/* Отключение от сервера */
void TCPclient::DisconnectFromHost()
{
    if(socket->state() == QTcpSocket::ConnectedState){
        socket->disconnectFromHost();
    }
}

void TCPclient::ReadyReed()
{
    QDataStream incStream(socket);

    if(incStream.status() != QDataStream::Ok){
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("Ошибка открытия входящего потока для чтения данных!");
        msg.exec();
    }

    //Читаем до конца потока
    while(incStream.atEnd() == false){
         //Если мы обработали предыдущий пакет, мы скинули значение
        if(servHeader.idData == 0){
            //Проверяем количество полученных байт. Если доступных байт меньше чем
            //заголовок, то выходим из обработчика и ждем новую посылку. Каждая новая
            //посылка дописывает данные в конец буфера
            if(socket->bytesAvailable() < sizeof(ServiceHeader)){
                return;
            }
            else{
                //Читаем заголовок
                incStream >> servHeader;
                //Проверяем на корректность данных. Принимаем решение по заранее известному ID
                                //пакета. Если он "битый" отбрасываем все данные в  поисках нового
                if(servHeader.id != ID){
                    uint16_t hdr = 0;
                    while(!incStream.atEnd()){
                        incStream >> hdr;
                        if(hdr == ID){
                            incStream >> servHeader.idData;
                            incStream >> servHeader.status;
                            incStream >> servHeader.len;
                            break;
                        }
                    }
                }
            }
        }

        //Если получены не все данные, то выходим из обработчика. Ждем новую посылку
        if(socket->bytesAvailable() < servHeader.len){
            return;
        }
        else{
            //Обработка данных
            ProcessingData(servHeader, incStream);
            servHeader.idData = 0;
            servHeader.status = 0;
            servHeader.len = 0;
        }
    }
}

/* Обработка полученных данных */
void TCPclient::ProcessingData(ServiceHeader header, QDataStream &stream)
{
    switch (header.idData){
        case GET_TIME:
        {
            QDateTime dateTime;
            stream >> dateTime;
            emit sig_sendTime(dateTime);
            break;
        }
        case GET_SIZE:
        {
            uint32_t freeSpace;
            stream >> freeSpace;
            emit sig_sendFreeSize(freeSpace);
            break;
        }
        case GET_STAT:
        {
            StatServer stat;
            stream >> stat.incBytes;
            stream >> stat.sendBytes;
            stream >> stat.revPck;
            stream >> stat.sendPck;
            stream >> stat.workTime;
            stream >> stat.clients;
            emit sig_sendStat(stat);
            break;
        }
        case SET_DATA:
        {
            QString reply;
            stream >> reply;
            emit sig_SendReplyForSetData(reply);
            emit sig_Success(SET_DATA);
            break;
        }
        case CLEAR_DATA:
        {
            emit sig_Success(CLEAR_DATA);
            break;
        }
        default:
            emit sig_Error(ERR_NO_FUNCT);
            return;
    }
}
