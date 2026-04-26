#include <QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Запуск приложения";

    // 1. Создаем экземпляр класса из сетевого модуля
    QTcpSocket socket;
    socket.connectToHost("www.Test.com", 80);
    qDebug() << "Создан TCP сокет, состояние:" << socket.state();

    // Проверяем состояние сокета
    if (socket.state() == QTcpSocket::ConnectingState) {
        qDebug() << "Сокет в процессе подключения...";
    } else if (socket.state() == QTcpSocket::ConnectedState) {
        qDebug() << "Сокет подключен!";
    } else {
        qDebug() << "Сокет не подключен";
    }

    // 2. Создаем экземпляр класса из модуля БД
    QSqlDatabase db = QSqlDatabase::addDatabase("Tested_DB");
    db.setDatabaseName(":memory:");                              // Используем базу в ОЗУ

    qDebug() << "Создана база данных. Драйвер:" << db.driverName();

    // Открываем базу данных
    if (db.open()) {
        qDebug() << "База данных успешно открыта";

        // Создаем простую таблицу для демонстрации
        QSqlQuery query;
        if (query.exec("CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT)")) {
            qDebug() << "Таблица создана успешно";

            // Добавляем тестовые данные
            query.prepare("INSERT INTO test (name) VALUES (:name)");
            query.bindValue(":name", "Тестовое имя");
            if (query.exec()) {
                qDebug() << "Данные добавлены успешно";
            }
        }

        db.close();
        qDebug() << "База данных закрыта";
    }
    qDebug() << "Программа завершена";

    return 0;
}
