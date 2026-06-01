#ifndef INDEXER_H
#define INDEXER_H

#include <QString>
#include <QStringList>
#include <QSet>

//Класс индексатора файлов.
class Indexer
{
public:
    Indexer();
    //Загрузка конфигурации из ini-файла
    bool loadConfig(const QString& configPath);
    //Запуск процесса индексации
    void startIndexing();
    
private:
    //Рекурсивная обработка каталога
    void processDirectory(const QString& directory);
    //Обработка одного файла
    void processFile(const QString& filePath);
    //Очистка текста от лишних символов
    QString cleanText(const QString& text);
    //Подсчет частоты слов в тексте
    QMap<QString, int> calculateWordFrequency(const QString& text);
    //Проверка, нужно ли индексировать файл
    bool shouldIndexFile(const QString& filePath) const;
    
    QStringList m_directories;
    QStringList m_extensions;
    bool m_configLoaded;
};

#endif // INDEXER_H
