#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../Database/DatabaseManager.h"
#include <QMessageBox>
#include <QDebug>

inline QString ru(const char* text) {
    return QString::fromLocal8Bit(text);
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
    ui->searchButton->setEnabled(!words.isEmpty() && words.size() <= 4);
}

//Обработчик нажатия кнопки поиска или Enter
void MainWindow::onSearch()
{
    QString query = ui->searchInput->text().trimmed();
    QStringList words = query.split(" ", Qt::SkipEmptyParts);
    
    //Пустой запрос
    if (words.isEmpty()) {
        QMessageBox::warning(this, ru("Предупреждение"),ru ("Пожалуйста, введите поисковый запрос"));
        return;
    }
    
    //Превышение максимального количества слов
    if (words.size() > 4) {
        QMessageBox::warning(this, ru("Предупреждение"),ru ("Максимальное количество слов в запросе - 4"));
        return;
    }
    
    // Каждое слово обрабатывается
    QStringList cleanedWords;
    for (QString word : words) {
        word = word.toLower();
        word.remove(QRegExp("[^\\w]"));
        // Фильтрация по длине
        if (word.length() >= 3 && word.length() <= 32) {
            cleanedWords.append(word);
        }
    }
    
    //После очистки не осталось слов
    if (cleanedWords.isEmpty()) {
        QMessageBox::warning(this,ru("Предупреждение"),
                           ru ("Пожалуйста, введите корректные слова (длина от 3 до 32 символов)"));
        return;
    }
    
    m_lastSearchWords = cleanedWords;

    // Выполняем поиск и отображаем результаты
    displaySearchResults();
}

//Отображение результатов поиска в текстовом поле
void MainWindow::displaySearchResults()
{
    DatabaseManager& db = DatabaseManager::instance();
    
    // Проверка подключения к базе данных
    if (!db.isConnected()) {
        QMessageBox::critical(this, ru("Ошибка"),ru ("Нет подключения к базе данных"));
        return;
    }
    
    //Поиск
    QList<SearchResult> results = db.searchWords(m_lastSearchWords, 10);
    
    // Очищаем текстовое поле перед выводом новых результатов
    ui->resultsText->clear();
    
    //Нет результатов
    if (results.isEmpty()) {
        ui->resultsText->append(ru("Документы, содержащие все слова запроса, не найдены."));
        return;
    }
    
    // Заголовок с запросом пользователя
    ui->resultsText->append(QString(ru("Результат поиска: %1\n")).arg(m_lastSearchWords.join(", ")));
     // Разделительная линия из 80 знаков "="
    ui->resultsText->append(QString(80, '='));
    
    // Вывод каждого результата
    for (int i = 0; i < results.size(); ++i) {
        const SearchResult& result = results[i];
        ui->resultsText->append(QString(ru("%1. %2 (Релевантность: %3)"))
                            .arg(i + 1)
                            .arg(result.filename)
                            .arg(result.relevance));
        ui->resultsText->append(QString(ru("   Путь: %1")).arg(result.path));
        ui->resultsText->append("");
    }
}

//Загрузка и отображение всех слов во вкладке "Словарь"
void MainWindow::loadWordsTab()
{
    DatabaseManager& db = DatabaseManager::instance();
    
    // Проверка подключения к базе данных
    if (!db.isConnected()) {
        ui->wordsTable->setRowCount(1);
        ui->wordsTable->setItem(0, 0, new QTableWidgetItem(ru("Нет подключения к базе данных")));
        return;
    }
    
    QMap<QString, int> words = db.getAllWordsWithFrequency();
    
    ui->wordsTable->setRowCount(words.size());
    int row = 0;
    
    // Проходим по всем словам в map и добавляем в таблицу
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
    
    //Нет подключения к БД
    if (!db.isConnected()) {
        ui->documentsTable->setRowCount(1);
        ui->documentsTable->setItem(0, 0, new QTableWidgetItem(ru("Нет подключения к базе данных")));
        return;
    }
    
    QList<DocumentInfo> documents = db.getAllDocuments();
    
    ui->documentsTable->setRowCount(documents.size());
    
    // Проходим по всем документам
    for (int i = 0; i < documents.size(); ++i) {
        ui->documentsTable->setItem(i, 0, new QTableWidgetItem(documents[i].filename));
        ui->documentsTable->setItem(i, 1, new QTableWidgetItem(documents[i].path));
    }
    
    ui->documentsTable->resizeColumnsToContents();
}
