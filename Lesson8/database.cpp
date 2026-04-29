#include "database.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    dataBase = new QSqlDatabase();
    tableModel = nullptr;
    queryModel = nullptr;
}

DataBase::~DataBase()
{
    if(tableModel) delete tableModel;
    if(queryModel) delete queryModel;
    delete dataBase;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);
}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());

    bool status = dataBase->open();
    emit sig_SendStatusConnection(status);
}

/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{
    if(dataBase->isOpen()){
        *dataBase = QSqlDatabase::database(nameDb);
        dataBase->close();
    }
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(QString request)
{
    if (!dataBase->isOpen()) {
        qDebug() << "База данных не открыта";
        return;
    }

    QString lowerRequest = request.toLower();

    if (lowerRequest == "all_films" || lowerRequest == "все фильмы") {
        QSqlTableModel *model = getAllFilmsModel();
        if (model && !model->lastError().isValid()) {
            emit sig_SendTableModel(model, requestAllFilms);
        }
    }
    else if (lowerRequest == "comedy_films" || lowerRequest == "комедии") {
        QSqlQueryModel *model = getComedyFilmsModel();
        if (model && !model->lastError().isValid()) {
            emit sig_SendQueryModel(model, requestComedy);
        }
    }
    else if (lowerRequest == "horror_films" || lowerRequest == "ужасы") {
        QSqlQueryModel *model = getHorrorFilmsModel();
        if (model && !model->lastError().isValid()) {
            emit sig_SendQueryModel(model, requestHorrors);
        }
    }
    else {
        qDebug() << "Неизвестная команда:" << request;
    }
}

QSqlTableModel* DataBase::getAllFilmsModel()
{
    if(tableModel){
        delete tableModel;
    }

    tableModel = new QSqlTableModel(this, *dataBase);
    tableModel->setTable("film");
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    tableModel->select();

    tableModel->setHeaderData(tableModel->fieldIndex("title"), Qt::Horizontal, "Название фильма");
    tableModel->setHeaderData(tableModel->fieldIndex("description"), Qt::Horizontal, "Описание фильма");

    return tableModel;
}

QSqlQueryModel* DataBase::getComedyFilmsModel()
{
    if(queryModel){
        delete queryModel;
    }

    queryModel = new QSqlQueryModel(this);

    QString query = "SELECT f.title, f.description FROM film f "
                    "JOIN film_category fc ON f.film_id = fc.film_id "
                    "JOIN category c ON c.category_id = fc.category_id "
                    "WHERE c.name = 'Comedy'";

    queryModel->setQuery(query, *dataBase);
    queryModel->setHeaderData(0, Qt::Horizontal, "Название фильма");
    queryModel->setHeaderData(1, Qt::Horizontal, "Описание фильма");

    return queryModel;
}

QSqlQueryModel* DataBase::getHorrorFilmsModel()
{
    if(queryModel){
        delete queryModel;
    }

    queryModel = new QSqlQueryModel(this);

    QString query = "SELECT f.title, f.description FROM film f "
                    "JOIN film_category fc ON f.film_id = fc.film_id "
                    "JOIN category c ON c.category_id = fc.category_id "
                    "WHERE c.name = 'Horror'";

    queryModel->setQuery(query, *dataBase);
    queryModel->setHeaderData(0, Qt::Horizontal, "Название фильма");
    queryModel->setHeaderData(1, Qt::Horizontal, "Описание фильма");

    return queryModel;
}

void DataBase::clearModels()
{
    if(tableModel){
        tableModel->clear();
    }
    if(queryModel){
        queryModel->clear();
    }
}
