
#include <QStringList>
#include <QDir>
#include <QTextStream>
#include <QFileDialog>
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
    note_changed  = false;
    ignore_change = false;

    connect( ui->mainline, SIGNAL(textChanged(QString)),
             search,       SLOT(search_update(QString)));
    connect( ui->mainline, &QLineEdit::returnPressed,
        [this]()
        {
            ui->textEdit->setFocus();
            QString linefile = ui->mainline->text();
            bool found = false;
            for( auto fname : *notelist )
            {
                if( fname == linefile )
                {
                    found = true;
                    break;
                }
            }
            if( !found )
            {
                note_changed = false;
                notefile = linefile;
                ui->statusBar->showMessage("Notefile: " + notefile);
                clearNote();
                // only created if user edits it now..
            }
        });


    connect( search, SIGNAL(search_results(QStringList)),
             this, SLOT(elideNotes(QStringList)));

    connect( ui->textEdit, &QTextEdit::textChanged,
        [this]()
        {
            if(ignore_change) return;
            note_changed = true;
        });

    connect( ui->pb_opt, &QPushButton::clicked,
        [this]()
        {
            if( note_changed )
            {
                switchNote(notefile);
                note_changed = false;
                clearNote();
            }
            ui->mainline->clear();
            search->search_clear();
            ui->doclist->setFocus();
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
            switchNote(item->text());
        });

    split = new QSplitter();
    ui->centralWidget->layout()->addWidget(split);
    split->addWidget(ui->doclist);
    split->addWidget(ui->textEdit);

    connect(ui->actionConfig, SIGNAL(triggered()),
            this, SLOT(configDir()) );
}


MainWindow::~MainWindow()
{
    delete ui;
    delete settings;
}

void MainWindow::configDir()
{
    ui->statusBar->showMessage("configDir");
    QString newdir = QFileDialog::getExistingDirectory(this,
      "Select new note directory",
      settings->value("notes_path").toString());
        ui->statusBar->showMessage("selected dir " + newdir);
    if( newdir.length() > 0 )
    {
        settings->setValue("notes_path", newdir);
        ui->doclist->setUpdatesEnabled(false);
        ui->textEdit->setUpdatesEnabled(false);
        ui->doclist->clear();
        clearNote();
        populateList();
        ui->doclist->setUpdatesEnabled(true);
        ui->textEdit->setUpdatesEnabled(true);
    }
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
    bool exact_title = false;
    QString fname = ui->mainline->text();

    ui->doclist->setUpdatesEnabled(false);

    for( int irow=0; irow<ui->doclist->count(); ++irow)
    {
        auto it = ui->doclist->item(irow);
        it->setHidden(!notefiles.contains( it->text() ));
        if( it->text() == fname )
        {
            exact_title = true;
        }
    }

    if( exact_title )
    {
        switchNote(fname);
    }
    else
    {
        if( notefiles.length()>0)
        {
            switchNote(notefiles.at(0));
        }
        else
        {
            // prepare to write the file if user edits the text edit
            notefile = fname;
            clearNote();
        }
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


void MainWindow::clearNote()
{
    bool ic = ignore_change;
    ignore_change = true;
    ui->textEdit->clear();
    ignore_change = ic;
}


void MainWindow::loadNote(const QString &fname)
{
    ui->textEdit->setUpdatesEnabled(false);

    // fires a textchanged which might create a file
    // unintentionally
    bool ic = ignore_change;
    ignore_change = true;
    ui->textEdit->clear();

    //TODO: textfile & length check... if too long, just load partial?
    QString fpath = getNotesPath() + "/" + fname;
    QFile ff(fpath);

    ff.open(QFile::ReadOnly);
    QString note;
    note = ff.readAll();
    ff.close();

    ui->textEdit->setText(note);
    ignore_change = ic;
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
}


void MainWindow::switchNote(const QString &fname)
{
    if( note_changed )
    {
        if( notefile.length() > 0 )
        {
            saveNote(notefile);
            if( !notelist->contains(notefile) )
            {
                *notelist << notefile;
                loadEntry(notefile);
                ui->doclist->addItem(notefile);
            }
            else
            {
                updateEntry(notefile);
            }
        }
    }
    else
    {
        loadNote(fname);
    }
    note_changed = false;
    notefile = fname;
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
        loadEntry(fname);
    };
    iterNotes(loadfunc);
}


void MainWindow::loadEntry(const QString &fname)
{
    QString fpath = getNotesPath() + "/" + fname;
    QFile ff(fpath);

    ff.open(QFile::ReadOnly);
    QString note;
    note = ff.readAll();
    ff.close();

    search->load_entry(fname, note);
}


void MainWindow::updateEntry(const QString &fname)
{
    QString fpath = getNotesPath() + "/" + fname;
    QFile ff(fpath);

    ff.open(QFile::ReadOnly);
    QString note;
    note = ff.readAll();
    ff.close();

    search->update_entry(fname, note);
}
