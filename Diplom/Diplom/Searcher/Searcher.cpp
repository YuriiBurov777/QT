#include "Searcher.h"
#include <QRegExp>

Searcher::Searcher()
{
}

//Разбор и очистка поискового запроса
QStringList Searcher::parseQuery(const QString& query)
{
    QStringList words = query.split(" ", Qt::SkipEmptyParts);
    QStringList cleanedWords;
    
    for (QString word : words) {
        word = cleanWord(word);
        if (!word.isEmpty()) {
            cleanedWords.append(word);
        }
    }
    
    return cleanedWords;
}

//Проверка валидности поискового запроса
bool Searcher::isValidQuery(const QStringList& words)
{
    return !words.isEmpty() && words.size() <= 4;
}

//Очистка отдельного слова от пунктуации и нормализация
QString Searcher::cleanWord(const QString& word)
{
    QString cleaned = word.toLower();
    cleaned.remove(QRegExp("[^\\w]"));
    
    if (cleaned.length() >= 3 && cleaned.length() <= 32) {
        return cleaned;
    }
    
    return QString();
}
