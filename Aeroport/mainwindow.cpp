#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statsdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(new DBManager(this))
    , m_reconnectTimer(new QTimer(this))
    , m_firstConnectionAttempt(true)
{
    ui->setupUi(this);

    setupConnections();
    setControlsEnabled(false);

    // Показывать начальный статус
    updateStatusLabel();

    // Начать попытку подключения
    attemptReconnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Настройка соединений
void MainWindow::setupConnections()
{
    connect(m_reconnectTimer, &QTimer::timeout, this, &MainWindow::attemptReconnection);

    // Установка диапазон дат
    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->dateEdit->setDate(QDate(2016, 8, 15));

    // Подключение пунктов меню
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "О программе",
                           QString::fromUtf8("Инспектор аэропортов версия 1.0\n\n"
                           "Приложение для просмотра расписания рейсов\n"
                           "и загруженности аэропортов.\n\n"
                           "Данные предоставлены Росавиацией.\n"
                           "Период данных: 2016-08-15 - 2017-09-14"
                           "ВНИМАНИЕ: Вся информация в базе данных является\n"
                           "конфиденциальной и охраняется Федеральным законом\n"
                           "№152-ФЗ \"О персональных данных\""));
    });
}

//Управление подключением к БД
void MainWindow::attemptReconnection()
{
    // Если уже подключены
    if (m_dbManager->isConnected()) {
        if (m_reconnectTimer->isActive()) {
            m_reconnectTimer->stop(); // Останавливаем таймер
        }

        if (m_firstConnectionAttempt) {
            m_firstConnectionAttempt = false;
            loadAirports();             // Загружаем список аэропортов
            setControlsEnabled(true);   // Разблокируем элементы
        }
        updateStatusLabel();
        return;
    }

    // Пытаемся подключиться
    if (m_dbManager->connectToDatabase()) {
        // Успешное подключение
        if (m_reconnectTimer->isActive()) {
            m_reconnectTimer->stop();
        }

        loadAirports();
        setControlsEnabled(true);
        updateStatusLabel();

        if (!m_firstConnectionAttempt) {
            QMessageBox::information(this, "Подключение",
                                    QString::fromUtf8("Подключение к базе данных восстановлено"));
        }
        m_firstConnectionAttempt = false;
        // Ошибка подключения
    } else {
        updateStatusLabel();

        if (m_firstConnectionAttempt) {
            QMessageBox::critical(this, "Ошибка подключения",
                                 QString::fromUtf8("Не удалось подключиться к базе данных:\n") +
                                 m_dbManager->getLastError() +
                                 QString::fromUtf8("\n\nПовторная попытка через 5 секунд..."));
            m_firstConnectionAttempt = false;
        }

        if (!m_reconnectTimer->isActive()) {
            m_reconnectTimer->start(5000);  // Повтор через 5 секунд
        }
    }
}

//Обновление статуса
void MainWindow::updateStatusLabel()
{
    if (m_dbManager->isConnected()) {
        ui->statusLabel->setText(QString::fromUtf8("Статус: Подключено"));
        ui->statusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->statusLabel->setText(QString::fromUtf8("Статус: Отключено"));
        ui->statusLabel->setStyleSheet("color: red; font-weight: bold;");
    }
}

//Управление элементами управления
void MainWindow::setControlsEnabled(bool enabled)
{
    ui->airportComboBox->setEnabled(enabled);
    ui->departureRadioButton->setEnabled(enabled);
    ui->arrivalRadioButton->setEnabled(enabled);
    ui->dateEdit->setEnabled(enabled);
    ui->showFlightsButton->setEnabled(enabled);
    ui->showStatsButton->setEnabled(enabled);
}

//Загрузка списка аэропортов
void MainWindow::loadAirports()
{
    QVector<Airport> airports = m_dbManager->getAirports();

    ui->airportComboBox->clear();

    for (const Airport& airport : airports) {
        ui->airportComboBox->addItem(airport.name, airport.code);
    }
}

//Обработчики изменения параметров
void MainWindow::on_airportComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->flightsTable->setRowCount(0);
}

void MainWindow::on_departureRadioButton_toggled(bool checked)
{
    Q_UNUSED(checked)
    ui->flightsTable->setRowCount(0);
}

void MainWindow::on_arrivalRadioButton_toggled(bool checked)
{
    Q_UNUSED(checked)
    ui->flightsTable->setRowCount(0);
}

void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    ui->flightsTable->setRowCount(0);
}

//Отображение рейсов
void MainWindow::on_showFlightsButton_clicked()
{
    updateFlightTable();
}

void MainWindow::updateFlightTable()
{
    // Проверка подключения
    if (!m_dbManager->isConnected()) {
        QMessageBox::warning(this, "Нет подключения",
                            QString::fromUtf8("Отсутствует подключение к базе данных"));
        return;
    }

    // Получение параметров из UI
    int airportIndex = ui->airportComboBox->currentIndex();
    if (airportIndex < 0) {
        QMessageBox::warning(this, "Ошибка", QString::fromUtf8("Выберите аэропорт"));
        return;
    }

    QString airportCode = ui->airportComboBox->currentData().toString();
    QDate date = ui->dateEdit->date();
    bool isDeparture = ui->departureRadioButton->isChecked();

    // Запрос к БД в зависимости от направления
    QVector<Flight> flights;
    if (isDeparture) {
        flights = m_dbManager->getDepartures(airportCode, date);
    } else {
        flights = m_dbManager->getArrivals(airportCode, date);
    }

    // Настройка таблицы
    ui->flightsTable->setRowCount(flights.size());

    // Установка заголовков
    QStringList headers;
    if (isDeparture) {
        headers.append(QString::fromUtf8("Номер рейса"));
        headers.append(QString::fromUtf8("Время вылета"));
        headers.append(QString::fromUtf8("Аэропорт назначения"));
    } else {
        headers.append(QString::fromUtf8("Номер рейса"));
        headers.append(QString::fromUtf8("Время прилета"));
        headers.append(QString::fromUtf8("Аэропорт отправления"));
    }
    ui->flightsTable->setHorizontalHeaderLabels(headers);

    // Заполнение таблицы данными
    for (int i = 0; i < flights.size(); ++i) {
        const Flight& flight = flights[i];
        ui->flightsTable->setItem(i, 0, new QTableWidgetItem(flight.flightNo));
        ui->flightsTable->setItem(i, 1, new QTableWidgetItem(flight.time));
        ui->flightsTable->setItem(i, 2, new QTableWidgetItem(flight.otherAirport));
    }

    ui->flightsTable->resizeColumnsToContents();

    // Если рейсов нет - временное сообщение
    if (flights.size() == 0) {
        ui->statusLabel->setText(QString::fromUtf8("Статус: Подключено | Рейсов не найдено"));
        QTimer::singleShot(3000, this, [this]() { updateStatusLabel(); });
    }
}

//Открытие окна статистики
void MainWindow::on_showStatsButton_clicked()
{
    // Проверка подключения
    if (!m_dbManager->isConnected()) {
        QMessageBox::warning(this, "Нет подключения",
                            QString::fromUtf8("Отсутствует подключение к базе данных"));
        return;
    }

    // Проверка выбора аэропорта
    int airportIndex = ui->airportComboBox->currentIndex();
    if (airportIndex < 0) {
        QMessageBox::warning(this, "Ошибка", QString::fromUtf8("Выберите аэропорт"));
        return;
    }

    // Получение данных о выбранном аэропорте
    QString airportCode = ui->airportComboBox->currentData().toString();
    QString airportName = ui->airportComboBox->currentText();

    // Создание и отображение диалога статистики
    StatsDialog* dialog = new StatsDialog(airportCode, airportName, m_dbManager, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}
