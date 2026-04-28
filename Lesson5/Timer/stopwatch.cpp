
#include "stopwatch.h"
#include <QElapsedTimer>

Stopwatch::Stopwatch(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(100); // 0.1 секунды

    // Подключаем сигнал таймера к слоту
    connect(m_timer, &QTimer::timeout, this, &Stopwatch::updateTime);

    m_lapCounter = 0;
    m_lastLapTime = 0;
}
// Запустить секундомер
void Stopwatch::start()
{
    m_elapsedTimer.start();
    m_timer->start();
}
// Остановить секундомер
void Stopwatch::stop()
{
    m_timer->stop();
    m_lastLapTime = 0;
}
// Сбросить секундомер
void Stopwatch::reset()
{
   // m_timer->stop();
    m_lapCounter = 0;
    emit timeUpdated("0.0");
}
//Вычисляет текущее время и отправляет сигнал
void Stopwatch::updateTime()
{
    int elapsed = m_elapsedTimer.elapsed() / 100; // в десятых долях секунды
    double seconds = elapsed / 10.0;
    emit timeUpdated(QString::number(seconds, 'f', 1));
}
//Вычисляет время от предыдущего круга и отправляет сигнал
QString Stopwatch::getLapTime()
{
    m_lapCounter++;
    int currentTime = m_elapsedTimer.elapsed() / 100;
    int lapTime = currentTime - m_lastLapTime;
    m_lastLapTime = currentTime;

    double lapSeconds = lapTime / 10.0;
    return QString("Круг %1, время: %2 сек")
           .arg(m_lapCounter)
           .arg(QString::number(lapSeconds, 'f', 1));
}
