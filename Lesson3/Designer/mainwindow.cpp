#include <QDebug>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    newWindow = new Dialog(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    newWindow->show();
    Dialog dialog(this);

    // Показываем диалог и ждем результат
    if (dialog.exec() == QDialog::Accepted) {
        // Пользователь нажал OK - получаем данные
        QString hostName = dialog.getHostName();
        QString dbName = dialog.getDatabaseName();
        QString login = dialog.getLogin();
        QString password = dialog.getPassword();
        uint port = dialog.getPort();

        // Выводим в консоль для отладки
        qDebug() << "--- Настройки подключения ---";
        qDebug() << "Хост:" << hostName;
        qDebug() << "База данных:" << dbName;
        qDebug() << "Логин:" << login;
        qDebug() << "Порт:" << port;
        qDebug() << "-----------------------------";
    }
}
