#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include "searcher.h"

static QSqlDatabase* s_db = 0;

Searcher::Searcher(QObject *parent) :
    QObject(parent)
{
    if( s_db == 0 )
    {
        s_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    }
    s_db->setHostName("localhost");
    s_db->setDatabaseName("tmp.db");
    s_db->open();

    QSqlQuery setuptable(*s_db);
    setuptable.exec("CREATE VIRTUAL TABLE docs USING fts4(fname,body);");
}

Searcher::~Searcher()
{
    s_db->close();
}

void Searcher::load_entry( const QString &title, const QString &body)
{
    static QSqlQuery load(*s_db);
    static bool do_prep = true;
    if( do_prep )
    {
        do_prep = false;
        load.prepare("INSERT INTO docs(fname,body) VALUES (:fname, :body);");
    }
    load.bindValue( ":fname", QVariant(title) );
    load.bindValue( ":body", QVariant(body) );
    load.exec();
}

void Searcher::search_clear()
{
    emit search_status("Search clear");
}

void Searcher::search_update(QString str)
{
    emit search_status("Searching: " + str);
}
