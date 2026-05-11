#include "statsdialog.h"
#include "ui_statsdialog.h"
#include "qcustomplot.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>

StatsDialog::StatsDialog(const QString& airportCode,
                         const QString& airportName,
                         DBManager* dbManager,
                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatsDialog)
    , m_airportCode(airportCode)
    , m_airportName(airportName)
    , m_dbManager(dbManager)
    , m_yearlyPlot(nullptr)
    , m_monthlyPlot(nullptr)
    , m_monthComboBox(nullptr)
    , m_tabWidget(nullptr)
    , m_airportLabel(nullptr)
{
    ui->setupUi(this);

    // Очищаем стандартный layout из UI. P.S.:без этой строки не получается создать экран из кода
    delete ui->gridLayout;

    setupUI();

    // Подключаем кнопку закрытия из UI
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

StatsDialog::~StatsDialog()
{
    delete ui;
}

//Настройка окна
void StatsDialog::setupUI()
{
    setWindowTitle(QString::fromUtf8("Загруженность аэропорта")); // Заголовок
    setModal(true);                                               // Модальное окно (блокирует главное)
    setMinimumSize(800, 600);

    //Основной виджет
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);                                   // Расстояние между виджетами
    mainLayout->setContentsMargins(10, 10, 10, 10);               // Отступы от краев

    // Наименование аэропотра
    m_airportLabel = new QLabel(QString::fromUtf8("Аэропорт: ") + m_airportName, this);
    m_airportLabel->setAlignment(Qt::AlignCenter);
    QFont labelFont = m_airportLabel->font();
    labelFont.setPointSize(12);
    labelFont.setBold(true);
    m_airportLabel->setFont(labelFont);
    mainLayout->addWidget(m_airportLabel);

    // Виджет вкладок
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    // Вкладка год
    QWidget* yearlyTab = new QWidget(this);
    QVBoxLayout* yearlyLayout = new QVBoxLayout(yearlyTab);
    m_yearlyPlot = new QCustomPlot(yearlyTab);
    yearlyLayout->addWidget(m_yearlyPlot);
    m_tabWidget->addTab(yearlyTab, QString::fromUtf8("Загруженность за год"));

    // Вкладка месяц
    QWidget* monthlyTab = new QWidget(this);
    QVBoxLayout* monthlyLayout = new QVBoxLayout(monthlyTab);

    // Выбор месяца
    QHBoxLayout* monthSelectLayout = new QHBoxLayout();
    QLabel* monthLabel = new QLabel(QString::fromUtf8("Месяц:"), monthlyTab);
    monthSelectLayout->addWidget(monthLabel);

    m_monthComboBox = new QComboBox(monthlyTab);
    m_monthComboBox->addItem(QString::fromUtf8("Январь"));
    m_monthComboBox->addItem(QString::fromUtf8("Февраль"));
    m_monthComboBox->addItem(QString::fromUtf8("Март"));
    m_monthComboBox->addItem(QString::fromUtf8("Апрель"));
    m_monthComboBox->addItem(QString::fromUtf8("Май"));
    m_monthComboBox->addItem(QString::fromUtf8("Июнь"));
    m_monthComboBox->addItem(QString::fromUtf8("Июль"));
    m_monthComboBox->addItem(QString::fromUtf8("Август"));
    m_monthComboBox->addItem(QString::fromUtf8("Сентябрь"));
    m_monthComboBox->addItem(QString::fromUtf8("Октябрь"));
    m_monthComboBox->addItem(QString::fromUtf8("Ноябрь"));
    m_monthComboBox->addItem(QString::fromUtf8("Декабрь"));
    monthSelectLayout->addWidget(m_monthComboBox);
    monthSelectLayout->addStretch();
    monthlyLayout->addLayout(monthSelectLayout);

    //График для месяца
    m_monthlyPlot = new QCustomPlot(monthlyTab);
    monthlyLayout->addWidget(m_monthlyPlot);
    m_tabWidget->addTab(monthlyTab, QString::fromUtf8("Загруженность за месяц"));

    //Подключение сигналов
    connect(m_monthComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatsDialog::onMonthComboBoxChanged);

    //Кнопка закрытия
    mainLayout->addWidget(ui->closeButton);

    // Загрузка годовой статистики
    loadYearlyStats();

    // Загрузка января
    loadMonthlyStats(1);
}

//Годовая статистика. Столбчатая диаграмма
void StatsDialog::loadYearlyStats()
{
    if (!m_yearlyPlot) return;

    // Получение данных из БД
    QVector<MonthlyStat> stats = m_dbManager->getYearlyStats(m_airportCode);

    if (stats.isEmpty()) {
        m_yearlyPlot->clearGraphs();
        m_yearlyPlot->clearPlottables();
        m_yearlyPlot->replot();
        return;
    }

    // Подготовка данных
    QVector<double> ticks;
    QVector<double> values;

    for (int i = 0; i < stats.size(); ++i) {
        ticks.append(i);
        values.append(stats[i].count);
    }

    m_yearlyPlot->clearGraphs();
    m_yearlyPlot->clearPlottables();

    // Создание столбчатой диаграммы
    QCPBars* bars = new QCPBars(m_yearlyPlot->xAxis, m_yearlyPlot->yAxis);
    bars->setData(ticks, values);
    bars->setPen(QPen(Qt::blue));
    bars->setBrush(QBrush(QColor(0, 0, 255, 100)));

    // Настройка осей
    m_yearlyPlot->xAxis->setLabel(QString::fromUtf8("Месяцы"));
    m_yearlyPlot->yAxis->setLabel(QString::fromUtf8("Количество рейсов"));

    if (ticks.size() > 0) {
        m_yearlyPlot->xAxis->setRange(-0.5, ticks.size() - 0.5);
    }

    // Взаимодействие с графиком
    m_yearlyPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_yearlyPlot->rescaleAxes();

    // Добавление 10% запаса сверху
    if (m_yearlyPlot->yAxis->range().upper > 0) {
        m_yearlyPlot->yAxis->setRange(0, m_yearlyPlot->yAxis->range().upper * 1.1);
    }

    // Перерисовка
    m_yearlyPlot->replot();
}

//Месячная статистика
void StatsDialog::loadMonthlyStats(int month)
{
    // Получение данных по дням
    if (!m_monthlyPlot) return;

    QMap<int, int> dailyStats = m_dbManager->getMonthlyStats(m_airportCode, month);

    QVector<double> days;   // Ось X: числа месяца
    QVector<double> values; // Ось Y: количество рейсов

    // Определяем последний день с данными
    int maxDay = 0;
    QMap<int, int>::const_iterator it;
    for (it = dailyStats.constBegin(); it != dailyStats.constEnd(); ++it) {
        if (it.key() > maxDay && it.value() > 0) {
            maxDay = it.key();
        }
    }

    if (maxDay == 0) maxDay = 31;

    // Заполнение массивов для всех дней месяца
    for (int day = 1; day <= maxDay; ++day) {
        days.append(day);
        values.append(dailyStats[day]);
    }

    m_monthlyPlot->clearGraphs();
    m_monthlyPlot->addGraph();

    // Создание линейного графика
    m_monthlyPlot->graph(0)->setData(days, values);
    m_monthlyPlot->graph(0)->setPen(QPen(Qt::red, 2));
    m_monthlyPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    m_monthlyPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

    // Настройка осей
    m_monthlyPlot->xAxis->setLabel(QString::fromUtf8("Число месяца"));
    m_monthlyPlot->yAxis->setLabel(QString::fromUtf8("Количество рейсов"));
    m_monthlyPlot->xAxis->setRange(0, maxDay + 1);
    m_monthlyPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_monthlyPlot->graph(0)->rescaleAxes(true);

    // Добавление 10% запаса сверху
    if (m_monthlyPlot->yAxis->range().upper > 0) {
        m_monthlyPlot->yAxis->setRange(0, m_monthlyPlot->yAxis->range().upper * 1.1);
    }

    // Перерисовка
    m_monthlyPlot->replot();
}

//Автоматическое перестроение графика при смене месяца
void StatsDialog::onMonthComboBoxChanged(int index)
{
    if (index >= 0) {
        loadMonthlyStats(index + 1);
    }
}
