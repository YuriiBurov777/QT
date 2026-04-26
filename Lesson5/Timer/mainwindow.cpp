#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаем секундомер
    m_stopwatch = new Stopwatch(this);
    m_isRunning = false;

    // Устанавливаем начальное состояние
    ui->m_timeLabel->setText("0.0");
    ui->m_lapButton->setEnabled(false);

    // Подключаем сигналы от секундомера
    connect(m_stopwatch, &Stopwatch::timeUpdated,
            this, &MainWindow::updateTimeDisplay);
    connect(m_stopwatch, &Stopwatch::lapRecorded,
            this, &MainWindow::addLapToBrowser);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//  кнопка startStopButton
void MainWindow::on_m_startStopButton_clicked()
{
    if (m_isRunning) {
        // Останавливаем
        m_stopwatch->stop();
        ui->m_startStopButton->setText("Старт");
        ui->m_lapButton->setEnabled(false);
    } else {
        // Запускаем
        m_stopwatch->start();
        ui->m_startStopButton->setText("Стоп");
        ui->m_lapButton->setEnabled(true);
    }
    m_isRunning = !m_isRunning;
}

//  кнопка clearButton
void MainWindow::on_m_clearButton_clicked()
{
    m_stopwatch->reset();
    ui->m_lapsList->clear();
    ui->m_startStopButton->setText("Старт");
    ui->m_lapButton->setEnabled(false);
    m_isRunning = false;
}

//  кнопка lapButton
void MainWindow::on_m_lapButton_clicked()
{
    m_stopwatch->getLapTime();
}

// Обновляем время на экране
void MainWindow::updateTimeDisplay(const QString &time)
{
    ui->m_timeLabel->setText(time);
}

// Добавляем круг в браузер
void MainWindow::addLapToBrowser(const QString &lapInfo)
{
    ui->m_lapsList->append(lapInfo);
}


