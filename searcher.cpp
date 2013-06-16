#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include "searcher.h"

static QSqlDatabase* s_db = 0;
static QSqlQuery* s_qy = 0;

Searcher::Searcher(QObject *parent) :
    QObject(parent)
{
    if( s_db == 0 )
    {
        s_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    }
    QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    QDir datadir(dirs.at(0));
    if(!datadir.exists()  )
    {
        datadir.mkpath(dirs.at(0));
    }
    QString dbpath = datadir.path() + "/upnote.db";

    s_db->setHostName("localhost");
    s_db->setDatabaseName(dbpath);
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


void Searcher::update_entry( const QString &title, const QString &body )
{
    static QSqlQuery update(*s_db);
    static bool do_prep = true;
    if( do_prep )
    {
        do_prep = false;
        update.prepare("UPDATE docs SET body=:body WHERE fname=:fname;");
    }
    update.bindValue( ":fname", QVariant(title) );
    update.bindValue( ":body", QVariant(body) );
    update.exec();
}

void Searcher::search_clear()
{
    emit search_status("Search clear");
}

void Searcher::search_update(QString str)
{
    if( s_qy == 0 )
    {
        s_qy = new QSqlQuery(*s_db);
        s_qy->prepare("SELECT fname FROM docs WHERE docs MATCH :sterm;");
    }
    emit search_status("Searching: " + str);

    QString sstr = "*" + str + "*";
    s_qy->bindValue( ":sterm", QVariant(sstr));
    s_qy->exec();

    QStringList results;
    while(s_qy->next())
    {
        results << s_qy->value(0).toString();
    }
    emit search_results(results);
}
