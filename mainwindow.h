#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "searcher.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    Searcher* search;
};

#endif // MAINWINDOW_H
