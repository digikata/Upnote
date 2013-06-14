#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>
#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QListWidget>
#include <QSettings>

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

    QString getNotesPath();

public slots:
    void elideNotes(const QStringList&);

private:
    Ui::MainWindow *ui;
    Searcher* search;
    QStringList* notelist;
    QSettings* settings;
    QString notespath;

    // title, path
    typedef std::function<void (const QString&)> noteFunc;
    typedef std::function<void (QListWidgetItem*)> nitFunc;

    void iterNotes(noteFunc);
    void iterList(nitFunc);
    void loadNote( const QString& fname);
    void populateList();
};

#endif // MAINWINDOW_H
