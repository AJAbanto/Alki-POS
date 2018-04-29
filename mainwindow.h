#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QtSql>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRelationalTableModel>
#include <QCompleter>
#include <QString>
#include <QMessageBox>
#include <QSettings>
#include <QFontDatabase>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSqlDatabase db;                    //Declaring Db and tbl objects
    QSqlTableModel *custtbl;             // to be manipulated
    QSqlTableModel *prodtbl;
    QSqlRelationalTableModel *inventbl;
    QSqlRelationalTableModel *transtbl;
    int BarcodefontID;
    QFontDatabase fontdb;
    QFont BarFont;

    QDialog *settingDialogue;           //objects for the settings Dialogue box cause idk how to pass args to slots tbh
    QLineEdit *hst;
    QLineEdit *dbnme;
    QLineEdit *usr;
    QLineEdit *pswd;


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();
    void on_btncustsearch_clicked();

    void on_btn_addprod_clicked();

    void on_btnprodsearch_clicked();

    void on_pushButton_5_clicked();

    void on_inventsearchbttn_clicked();

    void on_txtinventsearch_returnPressed();

    void on_txtsearchprod_returnPressed();

    void on_txtbarcode_returnPressed();

    void on_btn_cashout_clicked();

    void on_buttonBox_accepted();

    void on_pushButton_3_clicked();

    void on_txtpassword_returnPressed();

    void on_txtbarcode2_returnPressed();

    void on_btnPrintAllBarcode_clicked();

    void on_pushButton_4_clicked();

    void on_btn_void_clicked();

    void on_prodlookup_returnPressed();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;

    void setCompleters();
    void setTables();
    void loadDBsettings();
    void openDBsettings();
    bool createconnection();
    void printreceipt(float cashamount);
    void deletetrans();
    void getsubtotal();

};



#endif // MAINWINDOW_H
