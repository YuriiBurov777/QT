#include "Indexer.h"
#include "../Database/DatabaseManager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDebug>
#include <QRegExp>

Indexer::Indexer() : m_configLoaded(false)
{
}

//Загрузка конфигурации из ini-файла
bool Indexer::loadConfig(const QString& configPath)
{
    // Открываем файл в текстовом режиме
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть config.ini";
        return false;
    }

    // Читаем весь файл в массив байт
    QByteArray data = file.readAll();
    file.close();

    // Удаляем BOM если есть
    if (data.size() >= 3 && (unsigned char)data[0] == 0xEF &&
        (unsigned char)data[1] == 0xBB && (unsigned char)data[2] == 0xBF) {
        data = data.mid(3);
        qDebug() << "Удален UTF-8 BOM";
    }

    // Преобразуем байты в строку UTF-8
    QString content = QString::fromUtf8(data);

    // Парсим строки
    QStringList lines = content.split("\n");
    QString currentSection;

    for (QString line : lines) {
        line = line.trimmed();
        // Пропускаем пустые строки и комментарии
        if (line.isEmpty() || line.startsWith(";") || line.startsWith("#")) {
            continue;
        }

        //Проверка на секцию
        if (line.startsWith("[") && line.endsWith("]")) {
            currentSection = line.mid(1, line.length() - 2);
        }
        //Обработка параметров внутри секции Indexer
        else if (currentSection == "Indexer") {
            if (line.startsWith("Directories=")) {
                QString dirsStr = line.mid(12);
                m_directories = dirsStr.split(",", Qt::SkipEmptyParts);
                for (QString& dir : m_directories) {
                    dir = dir.trimmed();
                }
                qDebug() << "Загружены каталоги:" << m_directories;
            }
            else if (line.startsWith("FileExtensions=")) {
                QString extensionsStr = line.mid(15);
                m_extensions = extensionsStr.split(",", Qt::SkipEmptyParts);
                for (QString& ext : m_extensions) {
                    ext = ext.trimmed().toLower();
                }
                qDebug() << "Загружены расширения:" << m_extensions;
            }
        }
    }

    // Проверяем, что каталоги найдены
    if (m_directories.isEmpty()) {
        qDebug() << "Ошибка: Не найдены каталоги для индексации";
        return false;
    }

    // Проверяем, что расширения найдены
    if (m_extensions.isEmpty()) {
        qDebug() << "Ошибка: Не найдены расширения файлов";
        return false;
    }

    // Устанавливаем флаг успешной загрузки
    m_configLoaded = true;
    qDebug() << "Конфигурация загружена успешно!";
    return true;
}

//Запуск процесса индексации
void Indexer::startIndexing()
{
    // Проверка: загружена ли конфигурация?
    if (!m_configLoaded) {
        qDebug() << "Ошибка: Конфигурация не загружена";
        return;
    }
    
    // Получаем экземпляр менеджера БД (Singleton)
    DatabaseManager& db = DatabaseManager::instance();

    // Проверка: есть ли подключение к БД?
    if (!db.isConnected()) {
        qDebug() << "Ошибка: Нет подключения к базе данных";
        return;
    }
    
    //Очистка старого индекса
    qDebug() << "Очистка существующего индекса...";
    if (!db.clearIndex()) {
        qDebug() << "Ошибка: Не удалось очистить индекс";
        return;
    }
    
    qDebug() << "Начало индексации...";
    
    //Индексация всех каталогов из конфигурации
    for (const QString& directory : m_directories) {
        QDir dir(directory);
        if (dir.exists()) {
            qDebug() << "Индексация каталога:" << directory;
            processDirectory(directory);
        } else {
            qDebug() << "Предупреждение: Каталог не существует:" << directory;
        }
    }
    
    qDebug() << "Индексация успешно завершена!";
}

//Рекурсивный обход каталога
void Indexer::processDirectory(const QString& directory)
{
    QDir dir(directory);
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    // Обрабатываем каждый элемент
    for (const QFileInfo& entry : entries) {
        if (entry.isDir()) {
            // Рекурсивный вызов для подкаталога
            processDirectory(entry.absoluteFilePath());
        } else if (entry.isFile() && shouldIndexFile(entry.absoluteFilePath())) {

            // Индексация файла
            QString absolutePath = entry.absoluteFilePath();

            // Проверка на дубликат
            if (m_processedFiles.contains(absolutePath)) {
                qDebug() << "Пропущен дубликат:" << entry.fileName();
                continue;
            }

            m_processedFiles.insert(absolutePath);
            processFile(absolutePath);
        }
    }
}

//Проверка, нужно ли индексировать файл
bool Indexer::shouldIndexFile(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    return m_extensions.contains(extension);
}

//Индексация одного файла
void Indexer::processFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: Не удалось открыть файл:" << filePath;
        return;
    }
    
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    
    QString cleanedText = cleanText(content);
    QMap<QString, int> wordFrequency = calculateWordFrequency(cleanedText);
    
    // Если нет подходящих слов - пропускаем файл
    if (wordFrequency.isEmpty()) {
        qDebug() << "Предупреждение: В файле не найдено подходящих слов:" << QFileInfo(filePath).fileName();
        return;
    }
    
    DatabaseManager& db = DatabaseManager::instance();

    // Добавляем документ и получаем его ID
    int documentId = db.addDocument(filePath);
    
    if (documentId == -1) {
        qDebug() << "Ошибка: Не удалось добавить документ в базу данных:" << filePath;
        return;
    }
    
    //Сохраняем каждое слово и его частоту
    for (auto it = wordFrequency.begin(); it != wordFrequency.end(); ++it) {
        int wordId = db.addWord(it.key());
        if (wordId != -1) {
            // Добавляем вхождение: документ, слово, частота
            db.addWordOccurrence(documentId, wordId, it.value());
        }
    }
    
    qDebug() << "Проиндексирован:" << QFileInfo(filePath).fileName() 
             << "- Количество уникальных слов:" << wordFrequency.size();
}

//Очистка текста от лишних символов
QString Indexer::cleanText(const QString& text)
{
    QString result = text;
    
    // Нормализация пробелов
    result.replace(QRegExp("\\s+"), " ");
    
    // Удаление пунктуации
    result.remove(QRegExp("[^\\w\\s]"));
    
    // Приведение к нижнему регистру
    result = result.toLower();
    
    // Фильтрация слов по длине
    QStringList words = result.split(" ", Qt::SkipEmptyParts);
    QStringList filteredWords;
    
    // Проходим по всем словам
    for (const QString& word : words) {
        if (word.length() >= MIN_WORD_LENGTH && word.length() <= MAX_WORD_LENGTH) { //111
            filteredWords.append(word);
        }
    }
    
    // Собираем слова обратно в строку через пробелы
    return filteredWords.join(" ");
}

QMap<QString, int> Indexer::calculateWordFrequency(const QString& text)
{
    QMap<QString, int> frequency;
    QStringList words = text.split(" ", Qt::SkipEmptyParts);
    
    for (const QString& word : words) {
        frequency[word]++;
    }
    
    return frequency;
}

