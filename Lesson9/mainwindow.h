#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "udpworker.h"

#define TIMER_DELAY 1000

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_start_clicked();
    void DisplayTime(QDateTime data);
    void on_pb_stop_clicked();
    void on_pb_sendMessage_clicked();                                               // Новая кнопка для отправки сообщения
    void DisplayMessage(QString message, QHostAddress senderAddress, qint64 size);  // Новый слот для отображения сообщений

private:
    Ui::MainWindow* ui;
    QTimer* timer;
    UDPworker* udpWorker;
    uint32_t counterPck = 0;
};
#endif // MAINWINDOW_H
