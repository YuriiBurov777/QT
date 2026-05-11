#include "dbmanager.h"
#include <QDebug>

DBManager::DBManager(QObject *parent)
    : QObject(parent)
    , m_connected(false)
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("981757-ca08998.tmweb.ru");
    m_db.setPort(5432);
    m_db.setDatabaseName("demo");
    m_db.setUserName("netology_usr_cpp");
    m_db.setPassword("CppNeto3");
}

DBManager::~DBManager()
{
    disconnectFromDatabase();
}

bool DBManager::connectToDatabase()
{
    // Если уже открыто
    if (m_db.isOpen()) {
        m_connected = true;
        return true;
    }

    // Пытаемся открыть соединение
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        m_connected = false;
        qDebug() << "Ошибка подключения к базе данных:" << m_lastError;
        return false;
    }

    // Успешное подключение
    m_connected = true;
    m_lastError.clear();
    qDebug() << "База данных успешно подключена";
    return true;
}

//Отключение от БД
void DBManager::disconnectFromDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_connected = false;
}

//Проверка состояния подключения
bool DBManager::isConnected() const
{
    return m_connected && m_db.isOpen();
}

QString DBManager::getLastError() const
{
    return m_lastError;
}

//Получение списка аэропортов
QVector<Airport> DBManager::getAirports()
{
    QVector<Airport> airports;

    if (!isConnected()) {
        return airports; // Если нет подключения - пустой список
    }

    QSqlQuery query;
    query.exec("SELECT airport_code, airport_name->>'ru' as airportName FROM bookings.airports_data ORDER BY airportName");

    while (query.next()) {
        Airport airport;
        airport.code = query.value(0).toString();
        airport.name = query.value(1).toString();
        airports.append(airport);
    }

    return airports;
}

//Получение вылетов
QVector<Flight> DBManager::getDepartures(const QString& airportCode, const QDate& date)
{
    QVector<Flight> flights;

    if (!isConnected()) {
        return flights;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT f.flight_no,
               f.scheduled_departure::time as departure_time,
               ad.airport_name->>'ru' as arrival_airport
        FROM bookings.flights f
        JOIN bookings.airports_data ad ON ad.airport_code = f.arrival_airport
        WHERE f.departure_airport = :airportCode
          AND f.scheduled_departure::date = :date
        ORDER BY f.scheduled_departure
    )");

    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.exec();

    while (query.next()) {
        Flight flight;
        flight.flightNo = query.value(0).toString();
        flight.time = query.value(1).toTime().toString("HH:mm");
        flight.otherAirport = query.value(2).toString();
        flights.append(flight);
    }

    return flights;
}

//Получение прилетов
QVector<Flight> DBManager::getArrivals(const QString& airportCode, const QDate& date)
{
    QVector<Flight> flights;

    if (!isConnected()) {
        return flights;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT f.flight_no,
               f.scheduled_arrival::time as arrival_time,
               ad.airport_name->>'ru' as departure_airport
        FROM bookings.flights f
        JOIN bookings.airports_data ad ON ad.airport_code = f.departure_airport
        WHERE f.arrival_airport = :airportCode
          AND f.scheduled_arrival::date = :date
        ORDER BY f.scheduled_arrival
    )");

    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.exec();

    while (query.next()) {
        Flight flight;
        flight.flightNo = query.value(0).toString();
        flight.time = query.value(1).toTime().toString("HH:mm");
        flight.otherAirport = query.value(2).toString();
        flights.append(flight);
    }

    return flights;
}

//Получение годовой статистики
QVector<MonthlyStat> DBManager::getYearlyStats(const QString& airportCode)
{
    QVector<MonthlyStat> stats;

    if (!isConnected()) {
        return stats;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT COUNT(flight_no) as count,
               date_trunc('month', scheduled_departure) as month
        FROM bookings.flights f
        WHERE (scheduled_departure::date > DATE('2016-08-31')
               AND scheduled_departure::date <= DATE('2017-08-31'))
          AND (departure_airport = :airportCode OR arrival_airport = :airportCode)
        GROUP BY month
        ORDER BY month
    )");

    query.bindValue(":airportCode", airportCode);
    query.exec();

    static const QStringList monthNames = {
        "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
        "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"
    };

    while (query.next()) {
        MonthlyStat stat;
        stat.count = query.value(0).toInt();
        QDateTime monthDate = query.value(1).toDateTime();
        int monthIndex = monthDate.date().month() - 1;
        stat.month = monthNames[monthIndex];
        stats.append(stat);
    }

    return stats;
}

//Получение месячной статистики по дням
QMap<int, int> DBManager::getMonthlyStats(const QString& airportCode, int month)
{
    QMap<int, int> dailyStats;

    if (!isConnected()) {
        return dailyStats;
    }

    // Инициализируйте все дни месяца значением 0
    for (int day = 1; day <= 31; ++day) {
        dailyStats[day] = 0;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT COUNT(flight_no) as count,
               EXTRACT(DAY FROM scheduled_departure) as day
        FROM bookings.flights f
        WHERE EXTRACT(MONTH FROM scheduled_departure) = :month
          AND (scheduled_departure::date > DATE('2016-08-31')
               AND scheduled_departure::date <= DATE('2017-08-31'))
          AND (departure_airport = :airportCode OR arrival_airport = :airportCode)
        GROUP BY day
        ORDER BY day
    )");

    query.bindValue(":airportCode", airportCode);
    query.bindValue(":month", month);
    query.exec();

    while (query.next()) {
        int day = query.value(1).toInt();
        int count = query.value(0).toInt();
        dailyStats[day] = count;
    }

    return dailyStats;
}
