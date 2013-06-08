#include "searcher.h"

Searcher::Searcher(QObject *parent) :
    QObject(parent)
{
}

void Searcher::search_clear()
{
    emit search_status("Search clear");
}

void Searcher::search_update(QString str)
{
    emit search_status("Searching: " + str);
}
