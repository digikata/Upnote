#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>
#include <QStringList>
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

public slots:


private:
    Ui::MainWindow *ui;
    Searcher* search;
    QStringList* notelist;

    QString getNotesPath();

    // title, path
    typedef std::function<void (const QString&)> noteFunc;

    void iterNotes(noteFunc);
    void populateList();
};

#endif // MAINWINDOW_H
