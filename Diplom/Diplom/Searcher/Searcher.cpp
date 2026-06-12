#include "Searcher.h"
#include <QRegExp>

// Определение констант
constexpr int Searcher::MIN_WORD_LENGTH;
constexpr int Searcher::MAX_WORD_LENGTH;
constexpr int Searcher::MAX_WORDS_IN_QUERY;

Searcher::Searcher()
{
}

//Разбор и очистка поискового запроса
QStringList Searcher::parseQuery(const QString& query)
{
    QStringList words = query.split(" ", Qt::SkipEmptyParts);
    QStringList cleanedWords;

    for (const QString& word : words) {
        QString cleaned = cleanWord(word);
        if (!cleaned.isEmpty()) {
            cleanedWords.append(cleaned);
        }
    }

    return cleanedWords;
}

//Проверка валидности поискового запроса
bool Searcher::isValidQuery(const QStringList& words)
{
     return !words.isEmpty() && words.size() <= MAX_WORDS_IN_QUERY;
}

//Очистка отдельного слова от пунктуации и нормализация
QString Searcher::cleanWord(const QString& word)
{
    QString cleaned = word.toLower();
    cleaned.remove(QRegExp("[^\\w]"));
    
     if (cleaned.length() >= MIN_WORD_LENGTH && cleaned.length() <= MAX_WORD_LENGTH) {
        return cleaned;
    }
    
    return QString();
}
