#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    search = new Searcher(this);

    connect( ui->mainline, SIGNAL(textChanged(QString)),
             search,       SLOT(search_update(QString)));

    connect( search, SIGNAL(search_status(QString)),
             ui->statusBar, SLOT(showMessage(QString)));

    connect( ui->pb_opt, SIGNAL(clicked()),
             this, SLOT(clear_searchline()));

    ui->mainToolBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::clear_searchline()
{
    ui->mainline->clear();
    search->search_clear();
}
