#include "DatabaseManager.h"
#include <QSqlError>
#include <QDebug>
#include <QFileInfo>

//Деструктор - закрывает соединение с БД при уничтожении объекта
DatabaseManager::~DatabaseManager()
{
    disconnect();
}

//Реализация Singleton - создает единственный экземпляр класса
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager manager;  // Создается один раз при первом вызове
    return manager;
}

//Подключение к PostgreSQL с указанными параметрами
bool DatabaseManager::connect(const QString& host, int port, const QString& dbName,
                              const QString& username, const QString& password)
{
    // Проверяем, существует ли уже соединение с таким именем
    if (QSqlDatabase::contains("search_engine_connection")) {
        m_db = QSqlDatabase::database("search_engine_connection");
    } else {
        // Создаем новое соединение с PostgreSQL
        m_db = QSqlDatabase::addDatabase("QPSQL", "search_engine_connection");
    }
    
    // Устанавливаем параметры подключения
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(username);
    m_db.setPassword(password);
    
    // Пытаемся открыть соединение
    if (!m_db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << m_db.lastError().text();
        return false;
    }
    
    qDebug() << "Подключение к базе данных установлено";
    return true;
}

//Закрытие соединения с БД
void DatabaseManager::disconnect()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Соединение с базой данных закрыто";
    }
}

//Проверка состояния соединения
bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

//Создание структуры базы данных
bool DatabaseManager::createTables()
{
    QSqlQuery query(m_db);
    
    // Таблица документов
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS documents ("
        "id SERIAL PRIMARY KEY, "
        "path TEXT NOT NULL UNIQUE, "
        "filename TEXT NOT NULL"
        ")"
    );
    
    if (!success) {
        qDebug() << "Ошибка создания таблицы documents:" << query.lastError().text();
        return false;
    }
    
    // Таблица слов
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS words ("
        "id SERIAL PRIMARY KEY, "
        "word VARCHAR(32) NOT NULL UNIQUE"
        ")"
    );
    
    if (!success) {
        qDebug() << "Ошибка создания таблицы words:" << query.lastError().text();
        return false;
    }
    
    // Таблица вхождений (связь многие-ко-многим)
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS occurrences ("
        "document_id INTEGER REFERENCES documents(id) ON DELETE CASCADE, "
        "word_id INTEGER REFERENCES words(id) ON DELETE CASCADE, "
        "frequency INTEGER NOT NULL, "
        "PRIMARY KEY (document_id, word_id)"
        ")"
    );
    
    if (!success) {
        qDebug() << "Ошибка создания таблицы occurrences:" << query.lastError().text();
        return false;
    }
    
    // Создаем индексы для ускорения поиска
    query.exec("CREATE INDEX IF NOT EXISTS idx_occurrences_doc ON occurrences(document_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_occurrences_word ON occurrences(word_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_words_word ON words(word)");
    
    qDebug() << "Таблицы базы данных успешно созданы";
    return true;
}

//Полная очистка всех данных
bool DatabaseManager::clearIndex()
{
    QSqlQuery query(m_db);
    
    // Удаляем в правильном порядке из-за внешних ключей
    // Сначала дочернюю таблицу, потом родительские
    if (!query.exec("DELETE FROM occurrences")) {
        qDebug() << "Ошибка очистки таблицы occurrences:" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("DELETE FROM words")) {
        qDebug() << "Ошибка очистки таблицы words:" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("DELETE FROM documents")) {
        qDebug() << "Ошибка очистки таблицы documents:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Индекс успешно очищен";
    return true;
}

//Добавление документа
int DatabaseManager::addDocument(const QString& path)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO documents (path, filename) VALUES (?, ?) "
                  "ON CONFLICT (path) DO UPDATE SET filename = ? "
                  "RETURNING id");
    
    QFileInfo fileInfo(path);
    QString filename = fileInfo.fileName();
    
    query.addBindValue(path);
    query.addBindValue(filename);
    query.addBindValue(filename);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка добавления документа:" << query.lastError().text();
        return -1;
    }
    
    return query.value(0).toInt();
}

//Добавление слова
int DatabaseManager::addWord(const QString& word)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO words (word) VALUES (?) "
                  "ON CONFLICT (word) DO UPDATE SET word = ? "
                  "RETURNING id");
    
    query.addBindValue(word);
    query.addBindValue(word);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка добавления слова:" << query.lastError().text();
        return -1;
    }
    
    return query.value(0).toInt();
}

//Добавление вхождения слова в документе
bool DatabaseManager::addWordOccurrence(int documentId, int wordId, int frequency)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO occurrences (document_id, word_id, frequency) "
                  "VALUES (?, ?, ?) "
                  "ON CONFLICT (document_id, word_id) "
                  "DO UPDATE SET frequency = occurrences.frequency + ?");
    
    query.addBindValue(documentId);
    query.addBindValue(wordId);
    query.addBindValue(frequency);
    query.addBindValue(frequency);
    
    if (!query.exec()) {
        qDebug() << "Ошибка добавления вхождения слова:" << query.lastError().text();
        return false;
    }
    
    return true;
}

//Поиск документов, содержащих все указанные слова
QList<SearchResult> DatabaseManager::searchWords(const QStringList& words, int maxResults)
{
    QList<SearchResult> results;
    
    if (words.isEmpty()) {
        return results;
    }
    
    //Создаем плейсхолдеры для каждого слова
    QStringList placeholders;
    for (int i = 0; i < words.size(); ++i) {
        placeholders << "?";
    }

    // Формируем SQL запрос
    QString queryStr = 
        "SELECT d.id, d.path, d.filename, SUM(o.frequency) as total_relevance "
        "FROM documents d "
        "JOIN occurrences o ON d.id = o.document_id "
        "JOIN words w ON o.word_id = w.id "
        "WHERE w.word IN (" + placeholders.join(", ") + ") "
        "GROUP BY d.id, d.path, d.filename "
        "HAVING COUNT(DISTINCT w.word) = ? "
        "ORDER BY total_relevance DESC "
        "LIMIT ?";
    
    QSqlQuery query(m_db);
    query.prepare(queryStr);
    
    // Подставляем значения
    for (const QString& word : words) {
        query.addBindValue(word);
    }
    query.addBindValue(words.size());
    query.addBindValue(maxResults);
    
    if (!query.exec()) {
        qDebug() << "Ошибка выполнения поиска:" << query.lastError().text();
        return results;
    }
    
    // Формируем результаты
    while (query.next()) {
        SearchResult result;
        result.filename = query.value("filename").toString();
        result.path = query.value("path").toString();
        result.relevance = query.value("total_relevance").toInt();
        results.append(result);
    }
    
    return results;
}

//Получение всех слов с общей частотой
QMap<QString, int> DatabaseManager::getAllWordsWithFrequency()
{
    QMap<QString, int> wordsMap;
    
    QSqlQuery query(m_db);
    query.exec("SELECT w.word, COALESCE(SUM(o.frequency), 0) as total_freq "
               "FROM words w "
               "LEFT JOIN occurrences o ON w.id = o.word_id "
               "GROUP BY w.id, w.word "
               "ORDER BY total_freq DESC");
    
    while (query.next()) {
        QString word = query.value("word").toString();
        int frequency = query.value("total_freq").toInt();
        wordsMap[word] = frequency;
    }
    
    return wordsMap;
}

//Получение списка всех документов
QList<DocumentInfo> DatabaseManager::getAllDocuments()
{
    QList<DocumentInfo> documents;
    
    QSqlQuery query(m_db);
    query.exec("SELECT id, path, filename FROM documents ORDER BY filename");
    
    while (query.next()) {
        DocumentInfo doc;
        doc.id = query.value("id").toInt();
        doc.path = query.value("path").toString();
        doc.filename = query.value("filename").toString();
        documents.append(doc);
    }
    
    return documents;
}
