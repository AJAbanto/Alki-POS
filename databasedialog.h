#ifndef DATABASEDIALOG_H
#define DATABASEDIALOG_H

#include <QDialog>

namespace Ui {
class DatabaseDialog;
}

class DatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseDialog(QWidget *parent = 0);
    ~DatabaseDialog();

    QString getpass();
    QString getuser();
    QString gethost();
    QString getdbname();

    void setuser(QString user);
    void setpass(QString pass);
    void sethost(QString host);
    void setdb(QString db);

private:
    Ui::DatabaseDialog *ui;
};

#endif // DATABASEDIALOG_H
