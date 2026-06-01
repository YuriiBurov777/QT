#ifndef SEARCHER_H
#define SEARCHER_H

#include <QString>
#include <QStringList>

class Searcher
{
public:
    Searcher();
    
    QStringList parseQuery(const QString& query);
    bool isValidQuery(const QStringList& words);
    
private:
    QString cleanWord(const QString& word);
};

#endif // SEARCHER_H