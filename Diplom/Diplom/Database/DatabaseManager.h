#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QMap>

//Структура для хранения информации о документе
struct DocumentInfo {
    int id;
    QString path;
    QString filename;
};

//Структура для хранения информации о слове
struct WordInfo {
    int id;
    QString word;
    int totalFrequency;
};

//Структура для хранения результата поиска
struct SearchResult {
    QString filename;
    QString path;
    int relevance;
};

//Класс для управления базой данных PostgreSQL
class DatabaseManager
{
public:

    static DatabaseManager& instance();
    
    //Получение единственного экземпляра менеджера БД
    bool connect(const QString& host, int port, const QString& dbName,
                 const QString& username, const QString& password);
    //Отключение от базы данных
    void disconnect();
    //Проверка состояния подключения
    bool isConnected() const;
    //Создание всех необходимых таблиц в БД
    bool createTables();
    //Удаляет все записи из таблиц documents, words, occurrences.
    bool clearIndex();
    //Добавление документа в базу данных
    int addDocument(const QString& path);
    //Добавление слова в базу данных
    int addWord(const QString& word);
    //Добавление/обновление вхождения слова в документе
    bool addWordOccurrence(int documentId, int wordId, int frequency);
    //Поиск документов по списку слов
    QList<SearchResult> searchWords(const QStringList& words, int maxResults = 10);
    //Получение всех слов с их общей частотой
    QMap<QString, int> getAllWordsWithFrequency();
    //Получение списка всех документов
    QList<DocumentInfo> getAllDocuments();
    
private:
    DatabaseManager() = default;
    ~DatabaseManager();
    
    // Запрещаем копирование
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // Объект подключения к базе данных
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
