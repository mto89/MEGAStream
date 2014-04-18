#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
#include <QTemporaryFile>
#include <qDebug>
#include <QStringList>
#include <QProcess>
#include <QFileDialog>
#include <crypt.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void getAccountFiles(QString email, QString password); // read credential's files
    QString crypt(QString text, QString key, QString alphabet);   // crypt password on saving
    QString decrypt(QString text, QString key, QString alphabet); // decrypt password on loading
    bool boolAutoLoadPassword;
    QString myos; // operating system

private slots:
    void on_ffilter_textChanged(const QString &arg1);
    void on_refresh_clicked();
    void on_play_clicked();
    void on_EmptyAccount_clicked();
    void on_deleteAccount_clicked();
    void on_hidepwd_stateChanged(int arg1);
    void on_saveconf_clicked();
    void on_save_clicked();
    void on_load_clicked();
    void on_autopwd_stateChanged(bool checked);
    void on_txtMegals_textChanged(const QString &arg1);
    void on_txtPlayer_textChanged(const QString &arg1);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    QString megals;
    QString player;
    QString alphabet;
    Crypt objCrypt; // to crypt/decrypt password
    void on_actionNew_triggered();  // Reset table's cells
    void on_hidepwd_clicked(bool state);
    void openConf(QString filename);
    void openPass(QString filename);
    void savepass(QString fileName);
    void saveConf(QString filename);
};

#endif // MAINWINDOW_H
