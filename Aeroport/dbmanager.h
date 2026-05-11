#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QVector>
#include <QMap>

struct Airport {
    QString code;
    QString name;
};

struct Flight {
    QString flightNo;
    QString time;
    QString otherAirport;
};

struct MonthlyStat {
    QString month;
    int count;
};

struct DailyStat {
    int day;
    int count;
};

class DBManager : public QObject
{
    Q_OBJECT

public:
    explicit DBManager(QObject *parent = nullptr);
    ~DBManager();

    bool connectToDatabase();
    void disconnectFromDatabase();
    bool isConnected() const;
    QString getLastError() const;

    QVector<Airport> getAirports();
    QVector<Flight> getDepartures(const QString& airportCode, const QDate& date);
    QVector<Flight> getArrivals(const QString& airportCode, const QDate& date);
    QVector<MonthlyStat> getYearlyStats(const QString& airportCode);
    QMap<int, int> getMonthlyStats(const QString& airportCode, int month);

private:
    QSqlDatabase m_db;
    bool m_connected;
    QString m_lastError;
};

#endif // DBMANAGER_H
