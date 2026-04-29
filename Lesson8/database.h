#ifndef DATABASE_H
#define DATABASE_H

#include <QTableWidget>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

//Количество полей данных необходимых для подключения к БД
#define NUM_DATA_FOR_CONNECT_TO_DB 5

//Перечисление полей данных
enum fieldsForConnect{
    hostName = 0,
    dbName = 1,
    login = 2,
    pass = 3,
    port = 4
};

//Типы запросов
enum requestType{
    requestAllFilms = 0,
    requestComedy   = 1,
    requestHorrors  = 2
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void AddDataBase(QString driver, QString nameDB = "");
    void DisconnectFromDataBase(QString nameDb = "");
    void RequestToDB(QString request);
    void ConnectToDataBase(QVector<QString> dataForConnect);
    QSqlError GetLastError(void);


    QSqlTableModel* getAllFilmsModel();
    QSqlQueryModel* getComedyFilmsModel();
    QSqlQueryModel* getHorrorFilmsModel();
    void clearModels();

signals:
    void sig_SendDataFromDB(const QTableWidget *tableWg, int typeR);
    void sig_SendStatusConnection(bool);
    void sig_SendTableModel(QSqlTableModel* model, int typeR);
    void sig_SendQueryModel(QSqlQueryModel* model, int typeR);

private:
    QSqlDatabase* dataBase;
    QSqlTableModel* tableModel;
    QSqlQueryModel* queryModel;
};

#endif // DATABASE_H
