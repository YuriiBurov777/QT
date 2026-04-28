#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаем секундомер
    _StopWatch = new Stopwatch(this);
    m_isRunning = false;

    // Устанавливаем начальное состояние
    ui->m_timeLabel->setText("0.0");
    ui->lapButton->setEnabled(false);

    // Подключаем сигналы от секундомера
    connect(_StopWatch, &Stopwatch::timeUpdated,
            this, &MainWindow::updateTimeDisplay);
    //connect(m_stopwatch, &Stopwatch::lapRecorded,
     //       this, &MainWindow::addLapToBrowser);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//  кнопка startStopButton
void MainWindow::on_startStopButton_clicked()
{
    if (m_isRunning) {
        // Останавливаем
        _StopWatch->stop();
        ui->startStopButton->setText("Старт");
        ui->lapButton->setEnabled(false);
    } else {
        // Запускаем
        _StopWatch->start();
        ui->startStopButton->setText("Стоп");
        ui->lapButton->setEnabled(true);
    }
    m_isRunning = !m_isRunning;
}

//  кнопка clearButton
void MainWindow::on_clearButton_clicked()
{
    _StopWatch->reset();
    ui->m_lapsList->clear();
    //ui->m_startStopButton->setText("Старт");
   // ui->m_lapButton->setEnabled(false);
    //m_isRunning = false;
}

//  кнопка lapButton
void MainWindow::on_lapButton_clicked()
{
   // QString lapInfo = m_stopwatch->getLapTime();
    ui->m_lapsList->append(_StopWatch->getLapTime());
    //m_stopwatch->getLapTime();
}

// Обновляем время на экране
void MainWindow::updateTimeDisplay(const QString &time)
{
   ui->m_timeLabel->setText(time);
}

// Добавляем круг в браузер
//void MainWindow::addLapToBrowser(const QString &lapInfo)
//{
 //   //ui->m_lapsList->append(lapInfo);
//}


