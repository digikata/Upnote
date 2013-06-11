
#include <QStringList>
#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    search = new Searcher(this);
    notelist = 0;

    connect( ui->mainline, SIGNAL(textChanged(QString)),
             search,       SLOT(search_update(QString)));

    connect( search, SIGNAL(search_status(QString)),
             ui->statusBar, SLOT(showMessage(QString)));

    connect( ui->pb_opt, &QPushButton::clicked,
        [=]()
        {
            ui->mainline->clear();
            search->search_clear();
        });

    ui->mainToolBar->hide();
    populateList();

    connect( ui->doclist, &QListWidget::itemClicked,
        [=](QListWidgetItem* item)
        {
            loadNote(item->text());
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::getNotesPath()
{
    //TODO: replace with value from QSettings
    return QString("../corpus");
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

    ui->textEdit->setText(note);
    ui->textEdit->setUpdatesEnabled(true);
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
