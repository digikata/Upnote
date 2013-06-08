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
}

MainWindow::~MainWindow()
{
    delete ui;
}
