#include "Indexer.h"
#include "../Database/DatabaseManager.h"
#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Загрузка конфигурации базы данных
    QString configPath = "config.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    
    QString host = settings.value("Database/Host", "localhost").toString();
    int port = settings.value("Database/Port", 5432).toInt();
    QString dbName = settings.value("Database/DatabaseName").toString();
    QString username = settings.value("Database/Username").toString();
    QString password = settings.value("Database/Password").toString();
    
    if (dbName.isEmpty() || username.isEmpty()) {
        qDebug() << "Ошибка: Конфигурация базы данных в файле config.ini заполнена не полностью";
        return 1;
    }
    
    // Подключение к базе данных
    DatabaseManager& db = DatabaseManager::instance();
    if (!db.connect(host, port, dbName, username, password)) {
        qDebug() << "Ошибка: Не удалось подключиться к базе данных";
        return 1;
    }
    
    // Создание таблиц
    if (!db.createTables()) {
        qDebug() << "Ошибка: Не удалось создать таблицы в базе данных";
        return 1;
    }
    
    // Индексатор
    Indexer indexer;
    if (!indexer.loadConfig(configPath)) {
        qDebug() << "Ошибка: Не удалось загрузить конфигурацию индексатора";
        return 1;
    }
    
    qDebug() << "========================================";
    qDebug() << "Запуск индексатора поисковой системы";
    qDebug() << "========================================";
    indexer.startIndexing();
    qDebug() << "========================================";
    
    return 0;
}
