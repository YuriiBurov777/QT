#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);
    ui->pb_clear->setEnabled(false);

    currentRequestType = requestAllFilms;

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */
    dataDb = new DbData(this);
    dataBase = new DataBase(this);
    msg = new QMessageBox(this);

    //Установим размер вектора данных для подключения к БД
    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);

    // Устанавливаем данные для подключения по умолчанию
    dataForConnect[hostName] = "981757-ca08998.tmweb.ru";
    dataForConnect[dbName] = "netology_cpp";
    dataForConnect[login] = "netology_usr_cpp";
    dataForConnect[pass] = "CppNeto3";
    dataForConnect[port] = "5432";

    setupTableViews();

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    // Соединяем сигнал запроса с объединенным методом RequestToDB
    connect(this, &MainWindow::sig_RequestToDb, dataBase, &DataBase::RequestToDB);

    // Соединяем сигналы от БД
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(dataBase, &DataBase::sig_SendTableModel, this, &MainWindow::onReceiveTableModel);
    connect(dataBase, &DataBase::sig_SendQueryModel, this, &MainWindow::onReceiveQueryModel);
    connect(dataBase, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);

    /*
     * Устанавливаем данные для подключениея к БД.
     * Поскольку метод небольшой используем лямбда-функцию.
     */
    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTableViews()
{
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 */
void MainWindow::on_act_addData_triggered()
{
     //Отобразим диалоговое окно. Какой метод нужно использовать?
    dataDb->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 */
void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */
    if(ui->lb_statusConnect->text() == "Отключено"){
        ui->lb_statusConnect->setText("Подключение");
        ui->lb_statusConnect->setStyleSheet("color : black");

        auto conn = [&]{dataBase->ConnectToDataBase(dataForConnect);};
        QtConcurrent::run(conn);
    }
    else{
        dataBase->DisconnectFromDataBase("MyDB");
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
        ui->pb_clear->setEnabled(false);
        ui->tableView->setModel(nullptr);
    }
}

/*!
 * \brief Обработчик кнопки "Получить"
 */
void MainWindow::on_pb_request_clicked()
{
    // Используем единый сигнал с разными командами
    switch(currentRequestType){
        case requestAllFilms:
            emit sig_RequestToDb("ALL_FILMS");
            qDebug() << "Запрос: ВСЕ ФИЛЬМЫ";
            break;
        case requestComedy:
            emit sig_RequestToDb("COMEDY_FILMS");
            qDebug() << "Запрос: КОМЕДИИ";
            break;
        case requestHorrors:
            emit sig_RequestToDb("HORROR_FILMS");
            qDebug() << "Запрос: УЖАСЫ";
            break;
    }
}

void MainWindow::on_pb_clear_clicked()
{
    ui->tableView->setModel(nullptr);
}

void MainWindow::on_cb_category_currentIndexChanged(int index)
{
    // index: 0 - Все фильмы, 1 - Комедии, 2 - Ужасы
    switch(index) {
        case 0:
            currentRequestType = requestAllFilms;
            break;
        case 1:
            currentRequestType = requestComedy;
            break;
        case 2:
            currentRequestType = requestHorrors;
            break;
        default:
            currentRequestType = requestAllFilms;
            break;
    }
}

void MainWindow::onReceiveTableModel(QSqlTableModel *model, int typeR)
{
    if (!model) {
        qDebug() << "Модель таблицы пуста";
        return;
    }

    ui->tableView->setModel(model);

    // Скрываем все столбцы кроме title и description
    if (model->columnCount() > 0) {
        int titleCol = model->fieldIndex("title");
        int descCol = model->fieldIndex("description");

        // Показываем только нужные столбцы
        for (int i = 0; i < model->columnCount(); ++i) {
            if (i != titleCol && i != descCol) {
                ui->tableView->hideColumn(i);
            }
        }
    }

    ui->tableView->resizeColumnsToContents();
    ui->pb_clear->setEnabled(true);

    qDebug() << "Получена модель таблицы, тип запроса:" << typeR;
}

void MainWindow::onReceiveQueryModel(QSqlQueryModel *model, int typeR)
{
    if (!model) {
        qDebug() << "Модель запроса пуста";
        return;
    }

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->pb_clear->setEnabled(true);

    QString typeName = (typeR == requestComedy) ? "Комедии" : (typeR == requestHorrors) ? "Ужасы" : "Произвольный запрос";
    qDebug() << "Получена модель запроса, тип:" << typeName;
}

/*!
 * \brief Слот отображает значение в QTableWidget
 * \param widget
 * \param typeRequest
 */
void MainWindow::ScreenDataFromDB(const QTableWidget *widget, int typeRequest)
{
    if (widget) {
        qDebug() << "Получены данные из БД, тип запроса:" << typeRequest;
        qDebug() << "Количество строк:" << widget->rowCount();
        qDebug() << "Количество столбцов:" << widget->columnCount();
    }
}

/*!
 * \brief Метод изменяет стотояние формы в зависимости от статуса подключения к БД
 * \param status
 */
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if(status){
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else{
        dataBase->DisconnectFromDataBase("MyDB");
        msg->setIcon(QMessageBox::Critical);
        msg->setText(dataBase->GetLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg->exec();
    }
}

//void MainWindow::on_pushButton_clicked()
//{
//    qDebug() << "Текущий выбранный индекс:" << ui->cb_category->currentIndex();
//    qDebug() << "Текущий выбранный MW:" << currentRequestType;
//}


