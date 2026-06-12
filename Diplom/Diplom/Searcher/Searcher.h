#ifndef SEARCHER_H
#define SEARCHER_H

#include <QString>
#include <QStringList>

class Searcher
{
public:
    Searcher();

    // Константы для валидации
    static constexpr int MIN_WORD_LENGTH = 3;
    static constexpr int MAX_WORD_LENGTH = 32;
    static constexpr int MAX_WORDS_IN_QUERY = 4;

    static QStringList parseQuery(const QString& query);
    static bool isValidQuery(const QStringList& words);
    static QString cleanWord(const QString& word);

    static QStringList processQuery(const QString& query);
    static bool validateQuery(const QStringList& words);

private:
     QString m_lastQuery;
};

#endif // SEARCHER_H
