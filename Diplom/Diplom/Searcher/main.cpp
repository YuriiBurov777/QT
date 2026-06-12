#include "MainWindow.h"
#include "../Database/DatabaseManager.h"
#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QTextCodec>

inline QString ru(const char* text) {
    return QString::fromLocal8Bit(text);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Загрузка конфигурации базы данных
    QString configPath = "config.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    
    QString host = settings.value("Database/Host", "localhost").toString();
    int port = settings.value("Database/Port", 5432).toInt();
    QString dbName = settings.value("Database/DatabaseName").toString();
    QString username = settings.value("Database/Username").toString();
    QString password = settings.value("Database/Password").toString();
    
    if (dbName.isEmpty() || username.isEmpty()) {
        QMessageBox::critical(nullptr,ru("Ошибка"),
                            ru("Конфигурация базы данных в файле config.ini заполнена не полностью.\n"
                            "Пожалуйста, проверьте файл конфигурации и перезапустите приложение."));
        return 1;
    }
    
     // Подключение к базе данных
    DatabaseManager& db = DatabaseManager::instance();
    if (!db.connect(host, port, dbName, username, password)) {
        QMessageBox::critical(nullptr, ru("Ошибка"),
                            ru("Не удалось подключиться к базе данных.\n"
                            "Пожалуйста, проверьте настройки подключения и убедитесь, что PostgreSQL запущен.\n\n"
                            "Детали ошибки: Не удалось установить соединение с базой данных"));
        return 1;
    }
    
    // Проверка существования таблиц
    if (!db.createTables()) {
        QMessageBox::critical(nullptr,ru("Ошибка"),ru("Не удалось создать таблицы в базе данных"));
        return 1;
    }
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
