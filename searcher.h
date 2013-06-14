#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>
#include <QStringList>

class Searcher : public QObject
{
    Q_OBJECT
public:
    explicit Searcher(QObject *parent = 0);
    ~Searcher();

signals:
    void search_status(QString);
    void search_results(const QStringList&);

public slots:
    void load_entry(const QString& title, const QString& body);
    void search_clear();
    void search_update(QString);
};

#endif // SEARCHER_H
