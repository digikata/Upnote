#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>

class Searcher : public QObject
{
    Q_OBJECT
public:
    explicit Searcher(QObject *parent = 0);
    
signals:
    void search_status(QString);

public slots:
    void search_clear();
    void search_update(QString);
};

#endif // SEARCHER_H
