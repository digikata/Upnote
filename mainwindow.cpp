
#include <QStringList>
#include <QDir>
#include <QTextStream>
#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    search = new Searcher(this);
    notelist = 0;
    settings = 0;
    note_changed = false;
    note_create = false;

    connect( ui->mainline, SIGNAL(textChanged(QString)),
             search,       SLOT(search_update(QString)));

    connect( search, SIGNAL(search_results(QStringList)),
             this, SLOT(elideNotes(QStringList)));

    connect( ui->textEdit, &QTextEdit::textChanged,
        [this]()
        {
            note_changed = true;
            if( note_create )
            {
                note_create = false;
                notefile = ui->mainline.text();
                saveNote(notefile);
                ui->doclist.addItem(notefile);
                loadNote(notefile);
            }
        });

    connect( ui->pb_opt, &QPushButton::clicked,
        [this]()
        {
            ui->mainline->clear();
            search->search_clear();
            iterList( [](QListWidgetItem* it)
            {
                it->setHidden(false);
            });
        });

    ui->mainToolBar->hide();
    populateList();

    connect( ui->doclist, &QListWidget::itemClicked,
        [this](QListWidgetItem* item)
        {
            if( note_changed )
            {
                saveNote(notefile);
            }
            loadNote(item->text());
        });

    split = new QSplitter();
    ui->centralWidget->layout()->addWidget(split);
    split->addWidget(ui->doclist);
    split->addWidget(ui->textEdit);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete settings;
}


QString MainWindow::getNotesPath()
{
    if( settings == 0)
    {
        const QString orgval("Upnote");
        settings = new QSettings(QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 orgval, QString(), this );
    }
    notespath = settings->value("notes_path", QString("./notes") ).toString();
    return notespath;
}


void MainWindow::elideNotes(const QStringList & notefiles)
{
    ui->doclist->setUpdatesEnabled(false);
    if( notefiles.length()==0)
    {
        note_create = true;
    }
    else
    {
        loadNote(notefiles.at(0));
    }
    for( int irow=0; irow<ui->doclist->count(); ++irow)
    {
        auto it = ui->doclist->item(irow);
        it->setHidden(!notefiles.contains( it->text() ));
    }
    ui->doclist->setUpdatesEnabled(true);
}


void MainWindow::iterList(nitFunc nitfunc)
{
    ui->doclist->setUpdatesEnabled(false);
    for( int irow=0; irow<ui->doclist->count(); ++irow)
    {
       nitfunc(ui->doclist->item(irow));
    }
    ui->doclist->setUpdatesEnabled(true);
}


void MainWindow::iterNotes(noteFunc proc)
{
    QString notes_path = getNotesPath();

    QDir ndir(notes_path);
    if( !ndir.exists() )
    {
        ui->statusBar->showMessage("Couldn't open note path " + notes_path);
        return; // doesn't exist
    }

    QStringList flist = ndir.entryList( QDir::Files|QDir::Readable, QDir::Name);
    for( auto fname : flist )
    {
        proc(fname);
    }
}


void MainWindow::loadNote(const QString &fname)
{
    ui->textEdit->setUpdatesEnabled(false);
    ui->textEdit->clear();

    //TODO: textfile & length check... if too long, just load partial?
    QString fpath = getNotesPath() + "/" + fname;
    QFile ff(fpath);

    ff.open(QFile::ReadOnly);
    QString note;
    note = ff.readAll();
    ff.close();

    notefile = fname;
    note_changed = false;
    ui->textEdit->setText(note);
    ui->textEdit->setUpdatesEnabled(true);
}


void MainWindow::saveNote(const QString &fname)
{
    QString fpath = getNotesPath() + "/" + fname;
    QFile ff(fpath);
    QString note = ui->textEdit->toPlainText();
    ff.open(QFile::WriteOnly);
    QTextStream why(&ff);
    why << note;
    ff.close();
    note_changed = false;
}


void MainWindow::populateList()
{
    if( notelist==0 )
    {
        notelist = new QStringList();
    }

    ui->doclist->clear();
    auto plist = ui->doclist;
    auto nl = notelist;
    auto procfunc = [plist, nl](const QString& str)
    {
        nl->append(str);
        plist->addItem(nl->back());
    };

    iterNotes(procfunc);

    auto loadfunc = [this](const QString& fname)
    {
        QString fpath = getNotesPath() + "/" + fname;
        QFile ff(fpath);

        ff.open(QFile::ReadOnly);
        QString note;
        note = ff.readAll();
        ff.close();

        search->load_entry(fname, note);
    };
    iterNotes(loadfunc);
}
