#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

class Stopwatch : public QObject
{
    Q_OBJECT

public:
    explicit Stopwatch(QObject *parent = nullptr);

    void start();
    void stop();
    void reset();
    QString getLapTime();

signals:
    void timeUpdated(const QString &time);
    void lapRecorded(const QString &lapInfo);

private slots:
    void updateTime();

private:
    QTimer *m_timer;
    QElapsedTimer m_elapsedTimer;
    int m_lapCounter;
    int m_lastLapTime;
};

#endif
