#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>
#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QListWidget>
#include <QSettings>
#include <QSplitter>

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
    void configDir();

private:
    Ui::MainWindow *ui;
    Searcher* search;
    QStringList* notelist;
    QSplitter* split;
    QSettings* settings;
    QString notespath;
    bool note_changed;
    bool note_create;
    bool ignore_change;
    bool never_saved;
    QString notefile;

    // title, path
    typedef std::function<void (const QString&)> noteFunc;
    typedef std::function<void (QListWidgetItem*)> nitFunc;

    void iterNotes(noteFunc);
    void iterList(nitFunc);
    void populateList();

    void loadEntry(const QString& fname);
    void updateEntry(const QString& fname);
    
    void clearNote();
    void loadNote( const QString& fname);
    void saveNote( const QString& fname);
    void switchNote(const QString& fname);
};

#endif // MAINWINDOW_H
