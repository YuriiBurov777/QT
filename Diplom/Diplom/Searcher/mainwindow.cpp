#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../Database/DatabaseManager.h"
#include "Searcher.h"
#include <QMessageBox>
#include <QDebug>

// Константы для поиска
namespace {
    constexpr int MAX_SEARCH_RESULTS = 10;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подключение сигналов к слотам
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(ui->showDocumentsButton, &QPushButton::clicked, this, &MainWindow::onShowAllDocuments);
    connect(ui->searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(ui->searchInput, &QLineEdit::returnPressed, this, &MainWindow::onSearch);

    // Загружаем начальные данные
    loadWordsTab();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Валидация поискового запроса
void MainWindow::onSearchTextChanged(const QString& text)
{
    QStringList words = text.split(" ", Qt::SkipEmptyParts);
    // Используем константу из класса Searcher
    ui->searchButton->setEnabled(!words.isEmpty() && words.size() <= Searcher::MAX_WORDS_IN_QUERY);
}


//Обработчик нажатия кнопки поиска или Enter
void MainWindow::onSearch()
{
    QString query = ui->searchInput->text().trimmed();
    QStringList rawWords = query.split(" ", Qt::SkipEmptyParts);

    // Валидация  (Используем константу из класса Searcher)
    if (!Searcher::isValidQuery(rawWords)) {
        if (rawWords.isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", "Пожалуйста, введите поисковый запрос");
        } else if (rawWords.size() > Searcher::MAX_WORDS_IN_QUERY) {
            QMessageBox::warning(this, "Предупреждение",
                               QString("Максимальное количество слов в запросе - %1")
                               .arg(Searcher::MAX_WORDS_IN_QUERY));
        }
        return;
    }

    // Очистка слов (Используем константу из класса Searcher)
    QStringList cleanedWords = Searcher::parseQuery(query);

    if (cleanedWords.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                            QString("Пожалуйста, введите корректные слова (длина от %1 до %2 символов)")
                            .arg(Searcher::MIN_WORD_LENGTH)
                            .arg(Searcher::MAX_WORD_LENGTH));
        return;
    }

    m_lastSearchWords = cleanedWords;
    displaySearchResults();
}


//Отображение результатов поиска в текстовом поле
void MainWindow::displaySearchResults()
{
    DatabaseManager& db = DatabaseManager::instance();

    if (!db.isConnected()) {
        QMessageBox::critical(this, "Ошибка", "Нет подключения к базе данных");
        return;
    }

    // Используем константу
    QList<SearchResult> results = db.searchWords(m_lastSearchWords, MAX_SEARCH_RESULTS);

    ui->resultsText->clear();

    if (results.isEmpty()) {
        ui->resultsText->append("Документы, содержащие все слова запроса, не найдены.");
        return;
    }

    ui->resultsText->append(QString("Результаты поиска по запросу: %1\n").arg(m_lastSearchWords.join(", ")));
    ui->resultsText->append(QString(80, '='));

    for (int i = 0; i < results.size(); ++i) {
        const SearchResult& result = results[i];
        ui->resultsText->append(QString("%1. %2 (Релевантность: %3)")
                            .arg(i + 1)
                            .arg(result.filename)
                            .arg(result.relevance));
        ui->resultsText->append(QString("   Путь: %1").arg(result.path));
        ui->resultsText->append("");
    }
}

//Загрузка и отображение всех слов во вкладке "Словарь"
void MainWindow::loadWordsTab()
{
    DatabaseManager& db = DatabaseManager::instance();

    if (!db.isConnected()) {
        ui->wordsTable->setRowCount(1);
        ui->wordsTable->setItem(0, 0, new QTableWidgetItem("Нет подключения к базе данных"));
        return;
    }

    QMap<QString, int> words = db.getAllWordsWithFrequency();

    ui->wordsTable->setRowCount(words.size());
    int row = 0;

    for (auto it = words.begin(); it != words.end(); ++it) {
        ui->wordsTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->wordsTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        row++;
    }

    ui->wordsTable->resizeColumnsToContents();
}

//Обработчик кнопки "Показать все документы"
void MainWindow::onShowAllDocuments()
{
    loadDocumentsTab();
}

//Загрузка и отображение всех документов во вкладке "Документы"
void MainWindow::loadDocumentsTab()
{
    DatabaseManager& db = DatabaseManager::instance();

    if (!db.isConnected()) {
        ui->documentsTable->setRowCount(1);
        ui->documentsTable->setItem(0, 0, new QTableWidgetItem("Нет подключения к базе данных"));
        return;
    }

    QList<DocumentInfo> documents = db.getAllDocuments();

    ui->documentsTable->setRowCount(documents.size());

    for (int i = 0; i < documents.size(); ++i) {
        ui->documentsTable->setItem(i, 0, new QTableWidgetItem(documents[i].filename));
        ui->documentsTable->setItem(i, 1, new QTableWidgetItem(documents[i].path));
    }

    ui->documentsTable->resizeColumnsToContents();
}
